#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>
#include "datatype.h"
#include "meta.h"
#include "validator.h"
#include "../disk_storage/version_control.h"

class Cell {
    std::string value;
    DataType type;

public:
    Cell() noexcept;
    explicit Cell(std::string val) noexcept;
    Cell(std::string val, DataType explicit_type) noexcept;

    const std::string& get_value() const noexcept;
    DataType get_type() const noexcept;
    std::string_view get_type_name() const noexcept;
};

class Row {
    std::vector<Cell> cells;

public:
    explicit Row(const std::vector<std::string>& raw_cells);
    const std::vector<Cell>& get_cells() const noexcept;
    std::vector<Cell>& get_cells() noexcept;
};

class Table {
    std::string name;
    std::vector<Row> rows;
    Meta meta;

public:
    Table(const std::vector<std::string>& header, const std::vector<std::string>& data_types, std::string table_name);
    Table(const Table& other) = default;
    Table(Table&& other) noexcept = default;
    Table& operator=(const Table& other) = default;
    Table& operator=(Table&& other) noexcept = default;

    const std::string& get_name() const noexcept;
    void set_name(std::string new_name);
    std::vector<Row>& get_rows() noexcept;
    const std::vector<Row>& get_rows() const noexcept;
    void set_rows(std::vector<Row> new_rows);
    Meta& get_meta() noexcept;
    const Meta& get_meta() const noexcept;
};

class Database : public Observer {
    std::vector<Table> tables;
    std::unordered_map<std::string, size_t> table_index_map;
    static Database* instance;
    
    Database();

public:
    static Database* get_instance();
    Table* get_table_by_name(std::string_view name);
    std::vector<Table>& get_tables() noexcept;
    void create_table(const std::string& table_name, const std::vector<std::string>& headers, const std::vector<std::string>& data_types);
    void delete_table(std::string_view table_name, std::string_view row_idx_str);
    Table* read_table(std::string_view table_name, const std::vector<std::string>& headers, const std::vector<std::string>& where, const std::vector<std::string>& orderby);
    void insert_row(std::string_view table_name, const std::vector<std::string>& row);
    void insert_row(std::string_view table_name, const Row& row);
    void update_cell(std::string_view table_name, size_t row_idx, size_t col_idx, const std::string& value);
    void unionize_tables(const std::string& name, const std::vector<std::string>& table_names);
};