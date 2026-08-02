#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>
#include "query.h"
#include "lexer.h"
#include "dfa.h"

class Interpreter {
    enum class ActionType {
        CREATE, READ, INSERT, DELETE, UNION, UPDATE, LOAD, STORE, UNDO, UNKNOWN
    };

    std::unordered_map<std::string, ActionType> action_dispatch_map;
    Dfa dfa_create;
    Dfa dfa_read;
    Dfa dfa_insert;
    Dfa dfa_delete;
    Dfa dfa_union;
    Dfa dfa_update;
    Dfa dfa_command;

    Interpreter();

public:
    static Interpreter* get_instance() noexcept;
    void run(std::string_view queries, std::vector<Table*>* console_table_buffer);
    std::vector<std::string> get_all_tokens() const;
};