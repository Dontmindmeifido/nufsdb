#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"
#include "../interpreter/interpreter.h"
#include "../database/database.h"

class Interface {
    std::string query_buffer{"Insert Jane, Karera, 21, YES in GUESTS;"};
    std::vector<std::unique_ptr<Table>> console_table_buffer;
    int selected_table{-1};

    Interface() = default;
    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;
    Interface(Interface&&) = delete;
    Interface& operator=(Interface&&) = delete;
    ~Interface();

    void render_database_window() noexcept;
    void render_editor_window(float button_height) noexcept;
    void render_editor_console() noexcept;
    void render_table_view(const Table& table, const char* table_id) noexcept;

public:
    static Interface* get_instance() noexcept;
    GLFWwindow* init_window();
    void render(GLFWwindow* window);
};