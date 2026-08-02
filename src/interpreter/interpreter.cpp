#include "interpreter.h"
#include <iostream>

Interpreter::Interpreter()
    : dfa_create(2, {2, 1, 4, 0, 4, 3, 4, 4, 4, 4}, "in", 1),
      dfa_read(4, {1, 6, 6, 0, 6, 6, 6, 2, 6, 3, 4, 6, 6, 6, 6, 5, 6, 6, 6, 5, 6, 3, 4, 5, 6, 6, 6, 6}, "in", 3),
      dfa_insert(2, {1, 0, 3, 2, 3, 3, 3, 3}, "in", 1),
      dfa_update(2, {6, 1, 6, 2, 6, 3, 4, 6, 6, 5, 6, 6}, "in", 1),
      dfa_delete(2, {1, 0, 3, 2, 3, 3, 3, 3}, "in", 1),
      dfa_union(2, {1, 0, 3, 2, 3, 3, 3, 3}, "in", 1),
      dfa_command(1, {1, 2, 3, 3}, "", 0) {

    action_dispatch_map = {
        {"create", ActionType::CREATE},
        {"read",   ActionType::READ},
        {"insert", ActionType::INSERT},
        {"delete", ActionType::DELETE},
        {"union",  ActionType::UNION},
        {"update", ActionType::UPDATE},
        {"load",   ActionType::LOAD},
        {"store",  ActionType::STORE},
        {"undo",   ActionType::UNDO}
    };
}

Interpreter* Interpreter::get_instance() noexcept {
    static Interpreter instance;
    return &instance;
}

std::vector<std::string> Interpreter::get_all_tokens() const {
    return {"create", "read", "insert", "delete", "union", "update", "load", "store", "undo", "where", "orderby"};
}

void Interpreter::run(std::string_view queries, std::vector<Table*>* console_table_buffer) {
    if (!console_table_buffer) return;

    Lexer* lexer = Lexer::get_instance();
    auto instructions = lexer->tokenize(queries);

    for (std::vector<std::string>& token_stream : instructions) {
        if (token_stream.empty() || token_stream[0].empty()) continue;

        const std::string action_str = lexer->get_lower(token_stream[0]);
        auto query = QueryFactory::get_instance()->create_query(action_str);
        if (!query) continue;

        query->action = action_str;
        token_stream.erase(token_stream.begin());

        auto dispatch_it = action_dispatch_map.find(action_str);
        if (dispatch_it == action_dispatch_map.end()) continue;

        std::vector<int> states;
        std::string filter_mode;

        switch (dispatch_it->second) {
            case ActionType::CREATE:
                states = dfa_create.evaluate(token_stream);
                for (size_t i = 0; i < states.size(); ++i) {
                    if (states[i] == 0)      query->action_parameters1.push_back(std::move(token_stream[i]));
                    else if (states[i] == 1) query->action_parameters0.push_back(std::move(token_stream[i]));
                    else if (states[i] == 3) query->table = std::move(token_stream[i]);
                    else if (states[i] == 4) return;
                }
                break;

            case ActionType::READ:
                states = dfa_read.evaluate(token_stream);
                for (size_t i = 0; i < states.size(); ++i) {
                    if (states[i] == 0)      query->action_parameters0.push_back(std::move(token_stream[i]));
                    else if (states[i] == 2) query->table = std::move(token_stream[i]);
                    else if (states[i] == 3) filter_mode = "where";
                    else if (states[i] == 4) filter_mode = "orderby";
                    else if (states[i] == 5) {
                        if (filter_mode == "where")        query->filter_where_parameters.push_back(std::move(token_stream[i]));
                        else if (filter_mode == "orderby") query->filter_order_by_parameters.push_back(std::move(token_stream[i]));
                    }
                    else if (states[i] == 6) return;
                }
                break;

            case ActionType::INSERT:
                states = dfa_insert.evaluate(token_stream);
                for (size_t i = 0; i < states.size(); ++i) {
                    if (states[i] == 0)      query->action_parameters0.push_back(std::move(token_stream[i]));
                    else if (states[i] == 2) query->table = std::move(token_stream[i]);
                    else if (states[i] == 3) return;
                }
                break;

            case ActionType::DELETE:
            case ActionType::UNION:
                states = dfa_delete.evaluate(token_stream);
                for (size_t i = 0; i < states.size(); ++i) {
                    if (states[i] == 0)      query->action_parameters0.push_back(std::move(token_stream[i]));
                    else if (states[i] == 2) query->table = std::move(token_stream[i]);
                    else if (states[i] == 3) return;
                }
                break;

            case ActionType::UPDATE:
                states = dfa_update.evaluate(token_stream);
                for (size_t i = 0; i < states.size(); ++i) {
                    if (states[i] >= 1 && states[i] <= 3) query->action_parameters0.push_back(std::move(token_stream[i]));
                    else if (states[i] == 5)              query->table = std::move(token_stream[i]);
                    else if (states[i] == 6)              return;
                }
                break;

            case ActionType::LOAD:
            case ActionType::STORE:
                states = dfa_command.evaluate(token_stream);
                for (size_t i = 0; i < states.size(); ++i) {
                    if (states[i] == 1 || states[i] == 2) query->action_parameters0.push_back(std::move(token_stream[i]));
                    else if (states[i] == 3)              return;
                }
                break;

            default:
                break;
        }

        Table* query_response = query->execute(Database::get_instance());
        if (query_response) {
            console_table_buffer->push_back(query_response);
        }
    }
}