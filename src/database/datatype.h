#pragma once
#include <string>
#include <string_view>
#include <cctype>
#include <cstdint>

enum class DataType : uint8_t {
    VARCHAR = 0,
    NUMBER  = 1,
    DATETIME = 2,
    ANY     = 255
};

class Primitive {
public:
    static DataType parse_type_name(std::string_view type_name) noexcept;
    static std::string_view get_type_name_str(DataType type) noexcept;
    static bool is_number(std::string_view value) noexcept;
    static bool is_datetime(std::string_view value) noexcept;
    static DataType infer_type(std::string_view value) noexcept;
};