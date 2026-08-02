#include "datatype.h"
#include <algorithm>

DataType Primitive::parse_type_name(std::string_view type_name) noexcept {
    if (type_name == "VARCHAR")  return DataType::VARCHAR;
    if (type_name == "NUMBER")   return DataType::NUMBER;
    if (type_name == "DATETIME") return DataType::DATETIME;
    return DataType::ANY;
}

std::string_view Primitive::get_type_name_str(DataType type) noexcept {
    switch (type) {
        case DataType::VARCHAR:  return "VARCHAR";
        case DataType::NUMBER:   return "NUMBER";
        case DataType::DATETIME: return "DATETIME";
        default:                 return "ANY";
    }
}

bool Primitive::is_number(std::string_view value) noexcept {
    if (value.empty()) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isdigit(c);
    });
}

bool Primitive::is_datetime(std::string_view value) noexcept {
    if (value.size() != 10) return false;
    // Format: DD-MM-YYYY
    return std::isdigit(static_cast<unsigned char>(value[0])) &&
           std::isdigit(static_cast<unsigned char>(value[1])) &&
           value[2] == '-' &&
           std::isdigit(static_cast<unsigned char>(value[3])) &&
           std::isdigit(static_cast<unsigned char>(value[4])) &&
           value[5] == '-' &&
           std::isdigit(static_cast<unsigned char>(value[6])) &&
           std::isdigit(static_cast<unsigned char>(value[7])) &&
           std::isdigit(static_cast<unsigned char>(value[8])) &&
           std::isdigit(static_cast<unsigned char>(value[9]));
}

DataType Primitive::infer_type(std::string_view value) noexcept {
    if (is_datetime(value)) return DataType::DATETIME;
    if (is_number(value))   return DataType::NUMBER;
    return DataType::VARCHAR;
}