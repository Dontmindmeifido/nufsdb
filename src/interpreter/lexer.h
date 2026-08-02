#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "dfa.h"
#include "../misc/parser.h"

class Lexer : public Parser {
    Dfa dfa_tokenizer;

    Lexer();

public:
    static Lexer* get_instance() noexcept;
    std::vector<std::vector<std::string>> tokenize(std::string_view queries) const;
};