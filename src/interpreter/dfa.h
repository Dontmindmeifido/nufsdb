#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <cstdint>

class Dfa {
    std::vector<int> transitions;
    std::array<int, 256> char_to_alphabet;
    int alphabet_size;

public:
    Dfa(int alpha_size, 
        std::vector<int> transition_table, 
        std::string_view mapped_chars, 
        int default_token_idx) noexcept;

    std::vector<int> evaluate(std::string_view input) const;
    std::vector<int> evaluate(const std::vector<std::string>& tokens) const;
    int get_next_state(int current_state, unsigned char chr) const noexcept;
};