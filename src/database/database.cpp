#include "database.h"
#include <iostream>
#include <algorithm>
#include <charconv>

Cell::Cell() noexcept : value(""), type(DataType::VARCHAR) {}

Cell::Cell(std::string val) noexcept
    : value(std::move(val)), type(Primitive::infer_type(value)) {}

Cell::Cell(std::string val, DataType explicit_type) noexcept
    : value(std::move(val)), type(explicit_type) {}

const std::string& Cell::get_value() const noexcept { return value; }
DataType Cell::get_type() const noexcept { return type; }
std::string_view Cell::get_type_name() const noexcept {
    return Primitive::get_type_name_str(type);
}

Row::Row(const std::vector<std::string>& raw_cells) {
    cells.reserve(raw_cells.size());
    for (const auto& val : raw_cells) {
        cells.emplace_back(val);
    }
}

const std::vector<Cell>& Row::get_cells() const noexcept { return cells; }
std::vector<Cell>& Row::get_cells() noexcept { return cells; }

Table::Table(const std::vector<std::string>& header, const std::vector<std::string>& data_types, std::string table_name)
    : name(std::move(table_name)), meta(header, data_types) {
    rows.emplace_back(header);
}

const std::string& Table::get_name() const noexcept { return name; }
void Table::set_name(std::string new_name) { name = std::move(new_name); }
std::vector<Row>& Table::get_rows() noexcept { return rows; }
const std::vector<Row>& Table::get_rows() const noexcept { return rows; }
void Table::set_rows(std::vector<Row> new_rows) { rows = std::move(new_rows); }
Meta& Table::get_meta() noexcept { return meta; }
const Meta& Table::get_meta() const noexcept { return meta; }

Database* Database::instance = nullptr;

Database::Database() {
    this->subscribe(VersionControl::get_instance());
}

Database* Database::get_instance() {
    if (!instance) instance = new Database();
    return instance;
}

Table* Database::get_table_by_name(std::string_view name) {
    auto it = table_index_map.find(std::string(name));
    if (it != table_index_map.end()) {
        return &tables[it->second];
    }
    std::cout << "TABLE NOT FOUND\n";
    return nullptr;
}

std::vector<Table>& Database::get_tables() noexcept {
    return tables;
}

void Database::create_table(const std::string& table_name, const std::vector<std::string>& headers, const std::vector<std::string>& data_types) {
    if (headers.empty() || table_index_map.find(table_name) != table_index_map.end()) return;
    table_index_map[table_name] = tables.size();
    tables.emplace_back(headers, data_types, table_name);
}

void Database::delete_table(std::string_view table_name, std::string_view row_idx_str) {
    Table* table = get_table_by_name(table_name);
    if (!table) return;

    auto& rows = table->get_rows();
    if (rows.size() <= 1) return;

    size_t target_idx = rows.size() - 1;
    if (!row_idx_str.empty()) {
        size_t parsed_val = 0;
        auto [ptr, ec] = std::from_chars(row_idx_str.data(), row_idx_str.data() + row_idx_str.size(), parsed_val);
        if (ec == std::errc() && parsed_val >= 1 && parsed_val < rows.size()) {
            target_idx = parsed_val;
        } else {
            return;
        }
    }
    rows.erase(rows.begin() + target_idx);
}

Table* Database::read_table(std::string_view table_name, const std::vector<std::string>& headers, const std::vector<std::string>& where, const std::vector<std::string>& orderby) {
    Table* table = get_table_by_name(table_name);
    if (!table || headers.empty() || where.size() % 3 != 0 || orderby.size() % 2 != 0) return nullptr;

    const Meta& table_meta = table->get_meta();
    std::vector<size_t> col_indices;
    col_indices.reserve(headers.size());
    for (const auto& h : headers) {
        auto it = table_meta.column_index_map.find(h);
        if (it != table_meta.column_index_map.end()) {
            col_indices.push_back(it->second);
        }
    }
    if (col_indices.size() != headers.size()) return nullptr;

    const auto& src_rows = table->get_rows();
    std::vector<Row> result_rows;
    result_rows.reserve(src_rows.size());
    result_rows.emplace_back(headers);

    for (size_t i = 1; i < src_rows.size(); ++i) {
        const auto& cells = src_rows[i].get_cells();
        std::vector<std::string> projected;
        projected.reserve(col_indices.size());
        for (size_t idx : col_indices) {
            projected.push_back(cells[idx].get_value());
        }
        result_rows.emplace_back(projected);
    }

    if (!where.empty()) {
        const std::string& filter_col = where[0];
        const std::string& op = where[1];
        const std::string& comp_val = where[2];

        size_t col_idx = 0;
        bool found = false;
        for (size_t i = 0; i < headers.size(); ++i) {
            if (headers[i] == filter_col) {
                col_idx = i;
                found = true;
                break;
            }
        }
        if (!found) return nullptr;

        std::vector<Row> filtered_rows;
        filtered_rows.reserve(result_rows.size());
        filtered_rows.push_back(std::move(result_rows[0]));

        for (size_t i = 1; i < result_rows.size(); ++i) {
            const std::string& val = result_rows[i].get_cells()[col_idx].get_value();
            bool match = false;
            if (op == "=")       match = (val == comp_val);
            else if (op == ">")  match = (val > comp_val);
            else if (op == "<")  match = (val < comp_val);
            else if (op == ">=") match = (val >= comp_val);
            else if (op == "<=") match = (val <= comp_val);

            if (match) {
                filtered_rows.push_back(std::move(result_rows[i]));
            }
        }
        result_rows = std::move(filtered_rows);
    }

    if (!orderby.empty() && result_rows.size() > 1) {
        const std::string& sort_col = orderby[0];
        const std::string& direction = orderby[1];

        size_t col_idx = 0;
        for (size_t i = 0; i < headers.size(); ++i) {
            if (headers[i] == sort_col) {
                col_idx = i;
                break;
            }
        }

        if (direction == "descending") {
            std::sort(result_rows.begin() + 1, result_rows.end(), [col_idx](const Row& a, const Row& b) {
                return a.get_cells()[col_idx].get_value() > b.get_cells()[col_idx].get_value();
            });
        } else if (direction == "ascending") {
            std::sort(result_rows.begin() + 1, result_rows.end(), [col_idx](const Row& a, const Row& b) {
                return a.get_cells()[col_idx].get_value() < b.get_cells()[col_idx].get_value();
            });
        }
    }

    auto* ret_table = new Table(headers, {"ANY"}, "RESPONSE");
    ret_table->set_rows(std::move(result_rows));
    return ret_table;
}

void Database::insert_row(std::string_view table_name, const std::vector<std::string>& row) {
    Table* table = get_table_by_name(table_name);
    if (!table || row.empty()) return;

    Row new_row(row);
    if (!Validator::verify_column_size(table->get_meta(), new_row)) return;
    if (!Validator::verify_types(table->get_meta(), new_row)) return;

    table->get_rows().push_back(std::move(new_row));
}

void Database::insert_row(std::string_view table_name, const Row& row) {
    Table* table = get_table_by_name(table_name);
    if (!table) return;

    if (!Validator::verify_column_size(table->get_meta(), row)) return;
    if (!Validator::verify_types(table->get_meta(), row)) return;

    table->get_rows().push_back(row);
}

void Database::update_cell(std::string_view table_name, size_t row_idx, size_t col_idx, const std::string& value) {
    Table* table = get_table_by_name(table_name);
    if (!table) return;

    auto& rows = table->get_rows();
    if (row_idx >= rows.size()) return;

    auto& cells = rows[row_idx].get_cells();
    if (col_idx >= cells.size()) return;

    Cell new_cell(value);
    if (cells[col_idx].get_type() == new_cell.get_type() || table->get_meta().data_types[col_idx] == DataType::ANY) {
        cells[col_idx] = std::move(new_cell);
    }
}

void Database::unionize_tables(const std::string& name, const std::vector<std::string>& table_names) {
    if (table_names.empty() || table_index_map.find(name) != table_index_map.end()) return;

    Table* first_table = get_table_by_name(table_names[0]);
    if (!first_table) return;

    Table new_table(*first_table);
    new_table.set_name(name);

    for (size_t i = 1; i < table_names.size(); ++i) {
        Table* target_table = get_table_by_name(table_names[i]);
        if (!target_table) return;

        const auto& source_rows = target_table->get_rows();
        for (size_t j = 1; j < source_rows.size(); ++j) {
            new_table.get_rows().push_back(source_rows[j]);
        }
    }

    table_index_map[name] = tables.size();
    tables.push_back(std::move(new_table));
}