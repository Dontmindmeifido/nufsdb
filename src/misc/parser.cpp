#include "parser.h"
#include "../interpreter/interpreter.h"
#include <algorithm>
#include <cctype>

std::string Parser::get_lower(std::string_view value) noexcept {
    std::string result(value);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) noexcept {
        return std::tolower(c);
    });
    return result;
}

std::string Parser::get_stripped(std::string_view value) noexcept {
    std::string result;
    result.reserve(value.size());
    for (char c : value) {
        if (c != ' ' && c != '\n' && c != '\t' && c != ';') {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<std::string> Parser::get_partitioned_query(std::string_view value) noexcept {
    std::vector<std::string> tokens;
    tokens.reserve(32);

    std::string current;
    current.reserve(16);

    for (char c : value) {
        if (c == ';' || c == ',' || c == '(' || c == ')') {
            if (!current.empty()) {
                tokens.push_back(std::move(current));
                current.clear();
            }
            tokens.emplace_back(1, c);
        } else if (std::isspace(static_cast<unsigned char>(c))) {
            if (!current.empty()) {
                tokens.push_back(std::move(current));
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }

    if (!current.empty()) {
        tokens.push_back(std::move(current));
    }

    return tokens;
}

std::vector<std::string> Parser::get_spaced_words(std::string_view queries) noexcept {
    std::vector<std::string> stream;
    stream.reserve(64);

    std::string current;
    current.reserve(16);

    enum class CharClass { NONE, WORD, SPACE, NEWLINE, PUNCT };
    CharClass last_class = CharClass::NONE;

    auto get_class = [](char c) noexcept -> CharClass {
        if (c == '\n') return CharClass::NEWLINE;
        if (c == ' ' || c == '\t' || c == '\r') return CharClass::SPACE;
        if (c == ';' || c == ',' || c == '(' || c == '=' || c == '>' || c == '<') return CharClass::PUNCT;
        return CharClass::WORD;
    };

    for (char c : queries) {
        const CharClass cls = get_class(c);

        if (cls != last_class && !current.empty()) {
            stream.push_back(std::move(current));
            current.clear();
        }

        if (cls == CharClass::NEWLINE) {
            stream.emplace_back("\n");
            last_class = CharClass::NONE;
            continue;
        }

        current.push_back(c);
        last_class = cls;
    }

    if (!current.empty()) {
        stream.push_back(std::move(current));
    }

    return stream;
}

std::vector<std::string> Parser::get_snippets(std::string_view last_word, Database& db) {
    std::vector<std::string> ret;
    if (last_word.empty()) return ret;

    std::string_view target = last_word;
    while (!target.empty() && (target.front() == '(' || target.front() == ',' || target.front() == ' ')) {
        target.remove_prefix(1);
    }
    if (target.empty()) return ret;

    const std::string lower_target = get_lower(target);
    std::vector<std::string> candidates = Interpreter::get_instance()->get_all_tokens();

    for (const Table& table : db.get_tables()) {
        candidates.push_back(table.get_name());
        const auto& rows = table.get_rows();
        if (!rows.empty()) {
            for (const Cell& cell : rows[0].get_cells()) {
                candidates.push_back(cell.get_value());
            }
        }
    }

    for (const std::string& candidate : candidates) {
        if (candidate.size() >= lower_target.size()) {
            const std::string lower_candidate = get_lower(candidate.substr(0, lower_target.size()));
            if (lower_candidate == lower_target) {
                ret.push_back(candidate);
            }
        }
    }

    return ret;
}

std::pair<int, int> Parser::get_cursor_position(std::string_view query_buffer) noexcept {
    int x = 0;
    int y = 0;

    for (char chr : query_buffer) {
        if (chr == '\n') {
            y += 20;
            x = 0;
        } else {
            x += 8;
        }
    }

    return {x, y};
}