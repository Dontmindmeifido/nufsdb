#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include "../database/database.h"

class Parser {
public:
    virtual ~Parser() = default;

    static std::string get_lower(std::string_view value) noexcept;
    static std::string get_stripped(std::string_view value) noexcept;
    static std::vector<std::string> get_spaced_words(std::string_view queries) noexcept;
    static std::vector<std::string> get_partitioned_query(std::string_view value) noexcept;
    static std::vector<std::string> get_snippets(std::string_view last_word, Database& db);
    static std::pair<int, int> get_cursor_position(std::string_view query_buffer) noexcept;
};