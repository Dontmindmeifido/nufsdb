#include "interface.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

Interface::~Interface() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

Interface* Interface::get_instance() noexcept {
    static Interface instance;
    return &instance;
}

GLFWwindow* Interface::init_window() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "MyDB Engine", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    return window;
}

void Interface::render_table_view(const Table& table, const char* table_id) noexcept {
    const auto& rows = table.get_rows();
    if (rows.empty()) return;

    const auto& headers = rows[0].get_cells();
    const int col_count = static_cast<int>(headers.size());
    if (col_count == 0) return;

    const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable(table_id, col_count + 1, flags)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 30.0f);

        for (const Cell& header : headers) {
            ImGui::TableSetupColumn(header.get_value().c_str());
        }
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(rows.size()) - 1);

        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                const int row_idx = i + 1;
                const auto& cells = rows[row_idx].get_cells();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", row_idx);

                const int cell_len = std::min(col_count, static_cast<int>(cells.size()));
                for (int col = 0; col < cell_len; ++col) {
                    ImGui::TableSetColumnIndex(col + 1);
                    const Cell& cell = cells[col];

                    ImGui::TextUnformatted(cell.get_value().c_str());

                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("%s : %s", headers[col].get_value().c_str(), cell.get_type_name().data());
                        ImGui::Text("ROW %d COL %d", row_idx, col + 1);
                        ImGui::EndTooltip();
                    }
                }
            }
        }
        clipper.End();
        ImGui::EndTable();
    }
}

void Interface::render_database_window() noexcept {
    Database* db = Database::get_instance();
    const ImVec2 screen_size = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screen_size.x * 0.5f, screen_size.y), ImGuiCond_Always);

    ImGui::Begin("Database Viewer", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    auto& tables = db->get_tables();
    if (ImGui::BeginMenuBar()) {
        for (size_t i = 0; i < tables.size(); ++i) {
            if (ImGui::MenuItem(tables[i].get_name().c_str(), nullptr, selected_table == static_cast<int>(i))) {
                selected_table = static_cast<int>(i);
            }
        }
        ImGui::EndMenuBar();
    }

    if (selected_table >= 0 && selected_table < static_cast<int>(tables.size())) {
        render_table_view(tables[selected_table], "##db_main_table");
    }

    ImGui::End();
}

void Interface::render_editor_window(float button_height) noexcept {
    const ImVec2 screen_size = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(screen_size.x * 0.5f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screen_size.x * 0.5f, screen_size.y * 0.5f), ImGuiCond_Always);

    ImGui::Begin("Query Editor", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    Lexer* lexer = Lexer::get_instance();
    Database* db = Database::get_instance();

    std::string last_word;
    for (auto it = query_buffer.rbegin(); it != query_buffer.rend(); ++it) {
        if (*it != ' ' && *it != '\n' && *it != ';') {
            last_word.insert(last_word.begin(), *it);
        } else {
            break;
        }
    }
    const std::vector<std::string> snippets = lexer->get_snippets(last_word, *db);

    const ImVec2 editor_pos = ImGui::GetCursorScreenPos();
    const float editor_height = ImGui::GetContentRegionAvail().y - (button_height + ImGui::GetStyle().ItemSpacing.y);
    const ImVec2 editor_size = ImVec2(ImGui::GetContentRegionAvail().x, editor_height);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::InputTextMultiline(
        "##EditorInput",
        &query_buffer,
        editor_size,
        ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine
    );
    ImGui::PopStyleColor(2);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const float line_height = ImGui::GetTextLineHeight();
    ImVec2 cursor_offset = ImVec2(4.0f, 4.0f);

    std::vector<std::string> word_list = lexer->get_spaced_words(query_buffer);
    for (const std::string& token : word_list) {
        if (token.empty()) continue;

        if (token == "\n") {
            cursor_offset.x = 4.0f;
            cursor_offset.y += line_height;
            continue;
        }

        const std::string lower_token = lexer->get_lower(lexer->get_stripped(token));
        ImU32 text_color = IM_COL32(220, 220, 220, 255);

        if (lower_token == "create")       text_color = IM_COL32(50, 150, 200, 255);
        else if (lower_token == "insert")  text_color = IM_COL32(60, 200, 160, 255);
        else if (lower_token == "delete")  text_color = IM_COL32(200, 100, 50, 255);
        else if (lower_token == "read")    text_color = IM_COL32(200, 200, 220, 255);
        else if (lower_token == "union")   text_color = IM_COL32(200, 200, 140, 255);
        else if (lower_token == "update")  text_color = IM_COL32(200, 200, 70, 255);
        else if (lower_token == "where" || lower_token == "orderby") 
                                           text_color = IM_COL32(150, 100, 150, 255);
        else if (lower_token == "in")      text_color = IM_COL32(150, 100, 100, 255);
        else if (lower_token == "undo")    text_color = IM_COL32(50, 200, 60, 255);

        const ImVec2 render_pos = ImVec2(editor_pos.x + cursor_offset.x, editor_pos.y + cursor_offset.y);
        draw_list->AddText(render_pos, text_color, token.c_str());

        cursor_offset.x += ImGui::CalcTextSize(token.c_str()).x;
    }

    if (!snippets.empty() && !last_word.empty()) {
        const float popup_x = editor_pos.x + cursor_offset.x + 12.0f;
        const float popup_y = editor_pos.y + cursor_offset.y - 2.0f;

        float max_snippet_w = 0.0f;
        const size_t display_count = std::min<size_t>(3, snippets.size());
        for (size_t i = 0; i < display_count; ++i) {
            max_snippet_w = std::max(max_snippet_w, ImGui::CalcTextSize(snippets[i].c_str()).x);
        }

        const float card_width  = max_snippet_w + 24.0f;
        const float card_height = (display_count * line_height) + 12.0f;
        const ImVec2 card_min   = ImVec2(popup_x, popup_y);
        const ImVec2 card_max   = ImVec2(popup_x + card_width, popup_y + card_height);

        draw_list->AddRectFilled(
            ImVec2(card_min.x + 3.0f, card_min.y + 3.0f), 
            ImVec2(card_max.x + 3.0f, card_max.y + 3.0f), 
            IM_COL32(10, 10, 10, 180), 4.0f
        );
        draw_list->AddRectFilled(card_min, card_max, IM_COL32(28, 32, 38, 245), 4.0f);
        draw_list->AddRect(card_min, card_max, IM_COL32(80, 110, 140, 200), 4.0f, 0, 1.0f);

        for (size_t i = 0; i < display_count; ++i) {
            const ImVec2 item_pos = ImVec2(popup_x + 10.0f, popup_y + 6.0f + (i * line_height));
            draw_list->AddText(item_pos, IM_COL32(140, 210, 160, 255), snippets[i].c_str());
        }
    }

    if (ImGui::Button("Compile & Execute", ImVec2(160.0f, button_height))) {
        console_table_buffer.clear();
        std::vector<Table*> raw_out;
        Interpreter::get_instance()->run(query_buffer, &raw_out);

        console_table_buffer.reserve(raw_out.size());
        for (Table* tbl : raw_out) {
            console_table_buffer.emplace_back(tbl);
        }
    }

    ImGui::End();
}

void Interface::render_editor_console() noexcept {
    const ImVec2 screen_size = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(screen_size.x * 0.5f, screen_size.y * 0.5f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screen_size.x * 0.5f, screen_size.y * 0.5f), ImGuiCond_Always);

    ImGui::Begin("Execution Console", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    if (!console_table_buffer.empty() && console_table_buffer[0]) {
        render_table_view(*console_table_buffer[0], "##console_response_table");
    } else {
        ImGui::TextDisabled("No active execution output.");
    }

    ImGui::End();
}

void Interface::render(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render_database_window();
        render_editor_window(30.0f);
        render_editor_console();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.12f, 0.12f, 0.12f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}