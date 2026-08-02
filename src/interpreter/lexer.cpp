#include "lexer.h"

Lexer::Lexer()
    : dfa_tokenizer(
          6,
          {1, 1, 2, 2, 3, 0,
           1, 1, 2, 2, 3, 0,
           1, 1, 4, 4, 4, 0,
           1, 1, 4, 4, 4, 0,
           4, 4, 4, 4, 4, 4},
          " \n,:;%",
          5) {}

Lexer* Lexer::get_instance() noexcept {
    static Lexer instance;
    return &instance;
}

std::vector<std::vector<std::string>> Lexer::tokenize(std::string_view queries) const {
    std::vector<std::vector<std::string>> token_list;
    token_list.push_back({""});

    std::vector<int> states = dfa_tokenizer.evaluate(queries);
    for (size_t i = 0; i < states.size(); ++i) {
        auto& current_instruction = token_list.back();
        std::string& current_token = current_instruction.back();

        switch (states[i]) {
            case 0:
                current_token += queries[i];
                break;
            case 1:
            case 2:
                if (!current_token.empty()) {
                    current_instruction.emplace_back("");
                }
                break;
            case 3:
                if (!current_token.empty() || current_instruction.size() > 1) {
                    token_list.push_back({""});
                }
                break;
            default:
                break;
        }
    }

    if (!token_list.empty() && token_list.back().size() == 1 && token_list.back()[0].empty()) {
        token_list.pop_back();
    }

    return token_list;
}