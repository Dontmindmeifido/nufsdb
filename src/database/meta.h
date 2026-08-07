#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "datatype.h"

class Meta {
public:
    std::vector<DataType> data_types;
    std::vector<std::string> raw_type_names;
    std::unordered_map<std::string, size_t> column_index_map;
    size_t column_size = 0;

    Meta() = default;
    Meta(const std::vector<std::string>& headers, const std::vector<std::string>& types);
};