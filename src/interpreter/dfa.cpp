#include "dfa.h"
#include <algorithm>

Dfa::Dfa(int alpha_size, std::vector<int> transition_table, std::string_view mapped_chars, int default_token_idx) noexcept : transitions(std::move(transition_table)), alphabet_size(alpha_size) {
    char_to_alphabet.fill(default_token_idx);
    for (size_t i = 0; i < mapped_chars.size(); ++i) {
        unsigned char chr = static_cast<unsigned char>(mapped_chars[i]);
        char_to_alphabet[chr] = static_cast<int>(i);
    }
}

int Dfa::get_next_state(int current_state, unsigned char chr) const noexcept {
    const int symbol_idx = char_to_alphabet[chr];
    return transitions[current_state * alphabet_size + symbol_idx];
}

std::vector<int> Dfa::evaluate(std::string_view input) const {
    std::vector<int> state_sequence;
    state_sequence.reserve(input.size());

    int state = 0;
    for (unsigned char chr : input) {
        state = get_next_state(state, chr);
        state_sequence.push_back(state);
    }
    return state_sequence;
}

std::vector<int> Dfa::evaluate(const std::vector<std::string>& tokens) const {
    std::vector<int> state_sequence;
    state_sequence.reserve(tokens.size());

    int state = 0;
    for (const std::string& token : tokens) {
        unsigned char symbol = token.empty() ? '\0' : static_cast<unsigned char>(token[0]);
        const int symbol_idx = char_to_alphabet[symbol];
        state = transitions[state * alphabet_size + symbol_idx];
        state_sequence.push_back(state);
    }
    return state_sequence;
}