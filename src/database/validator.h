#pragma once
#include "meta.h"

class Validator {
public:
    template <typename RowType>
    static bool verify_types(const Meta& meta, const RowType& row) noexcept {
        if (!meta.data_types.empty() && meta.data_types[0] == DataType::ANY) {
            return true;
        }
        const auto& cells = row.get_cells();
        const size_t len = std::min(meta.data_types.size(), cells.size());
        for (size_t i = 0; i < len; ++i) {
            if (meta.data_types[i] != cells[i].get_type()) {
                return false;
            }
        }
        return true;
    }

    template <typename RowType>
    static bool verify_column_size(const Meta& meta, const RowType& row) noexcept {
        return meta.column_size == row.get_cells().size();
    }
};