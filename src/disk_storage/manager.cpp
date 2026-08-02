#include "manager.h"
#include <fstream>
#include <iostream>
#include <sstream>

Manager* Manager::get_instance() noexcept {
    static Manager instance;
    return &instance;
}

uint64_t Manager::fnv1a_hash(std::string_view data) noexcept {
    uint64_t hash = 0xCBF29CE484222325ULL;
    for (unsigned char c : data) {
        hash ^= c;
        hash *= 0x100000001B3ULL;
    }
    return hash;
}

void Manager::xor_stream_cipher(std::vector<char>& buffer, std::string_view key) noexcept {
    if (key.empty() || buffer.empty()) return;
    const size_t klen = key.size();
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] ^= key[i % klen];
    }
}

bool Manager::verify_credentials(const std::string& auth_path, std::string_view password) const {
    std::ifstream in(auth_path, std::ios::binary);
    if (!in.is_open()) return false;

    uint64_t stored_hash = 0;
    in.read(reinterpret_cast<char*>(&stored_hash), sizeof(stored_hash));
    return in.gcount() == sizeof(stored_hash) && stored_hash == fnv1a_hash(password);
}

void Manager::store_credentials(const std::string& auth_path, std::string_view password) const {
    std::ofstream out(auth_path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return;

    const uint64_t hash = fnv1a_hash(password);
    out.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
}

bool Manager::get_database(Database* database, std::string_view file_name, std::string_view password) {
    if (!database) return false;

    const std::string base_path = DIR_PREFIX + std::string(file_name);
    const std::string auth_path = base_path + HASH_EXT;

    if (!verify_credentials(auth_path, password)) {
        std::cerr << "Authentication error: Invalid database password.\n";
        return false;
    }

    std::ifstream in(base_path, std::ios::binary | std::ios::ate);
    if (!in.is_open()) return false;

    const std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> buffer(static_cast<size_t>(size));
    if (!in.read(buffer.data(), size)) return false;
    in.close();

    xor_stream_cipher(buffer, password);

    database->get_tables().clear();

    std::string value;
    std::string name;
    std::vector<std::string> headers;
    std::vector<std::string> data_types;
    std::vector<std::string> row_cells;

    for (char chr : buffer) {
        switch (chr) {
            case '\x00':
                name = std::move(value);
                value.clear();
                break;
            case '\x01':
                headers.push_back(std::move(value));
                value.clear();
                break;
            case '\x02':
                data_types.push_back(std::move(value));
                value.clear();
                break;
            case '\x03':
                database->create_table(name, headers, data_types);
                headers.clear();
                data_types.clear();
                value.clear();
                break;
            case '\x04':
                row_cells.push_back(std::move(value));
                value.clear();
                break;
            case '\x05':
                database->insert_row(name, row_cells);
                row_cells.clear();
                value.clear();
                break;
            default:
                value += chr;
                break;
        }
    }

    return true;
}

void Manager::save_database(Database* database, std::string_view file_name, std::string_view password) {
    if (!database) return;

    const std::string base_path = DIR_PREFIX + std::string(file_name);
    const std::string auth_path = base_path + HASH_EXT;

    store_credentials(auth_path, password);

    std::vector<char> buffer;
    buffer.reserve(4096);

    auto append_str = [&buffer](std::string_view str, char delimiter) {
        buffer.insert(buffer.end(), str.begin(), str.end());
        buffer.push_back(delimiter);
    };

    for (const Table& table : database->get_tables()) {
        append_str(table.get_name(), '\x00');

        const auto& rows = table.get_rows();
        if (!rows.empty()) {
            for (const Cell& header : rows[0].get_cells()) {
                append_str(header.get_value(), '\x01');
            }
        }

        for (DataType dt : table.get_meta().data_types) {
            append_str(Primitive::get_type_name_str(dt), '\x02');
        }

        buffer.push_back('\x03');

        for (size_t i = 1; i < rows.size(); ++i) {
            for (const Cell& cell : rows[i].get_cells()) {
                append_str(cell.get_value(), '\x04');
            }
            buffer.push_back('\x05');
        }
    }

    xor_stream_cipher(buffer, password);

    std::ofstream out(base_path, std::ios::binary | std::ios::trunc);
    if (out.is_open() && !buffer.empty()) {
        out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    }
}