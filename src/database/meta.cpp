#include "meta.h"

Meta::Meta(const std::vector<std::string>& headers, const std::vector<std::string>& types)
    : raw_type_names(types), column_size(types.size()) {
    data_types.reserve(column_size);
    for (size_t i = 0; i < column_size; ++i) {
        data_types.push_back(Primitive::parse_type_name(types[i]));
        if (i < headers.size()) {
            column_index_map[headers[i]] = i;
        }
    }
}