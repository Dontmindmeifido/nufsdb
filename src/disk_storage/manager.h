#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include "../database/database.h"

class Manager {
    static constexpr char DIR_PREFIX[] = "disk/";
    static constexpr char HASH_EXT[]   = ".auth";

    Manager() = default;

    static uint64_t fnv1a_hash(std::string_view data) noexcept;
    static void xor_stream_cipher(std::vector<char>& buffer, std::string_view key) noexcept;
    bool verify_credentials(const std::string& auth_path, std::string_view password) const;
    void store_credentials(const std::string& auth_path, std::string_view password) const;

public:
    static Manager* get_instance() noexcept;
    bool get_database(Database* database, std::string_view file_name, std::string_view password);
    void save_database(Database* database, std::string_view file_name, std::string_view password);
};