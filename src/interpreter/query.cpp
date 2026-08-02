#include "query.h"
#include <charconv>
#include <iostream>

Table* Create::execute(Database* database) {
    database->notify(database_file, undo_key);
    database->create_table(table, action_parameters0, action_parameters1);
    return nullptr;
}

Table* Insert::execute(Database* database) {
    database->notify(database_file, undo_key);
    database->insert_row(table, action_parameters0);
    return nullptr;
}

Table* Delete::execute(Database* database) {
    database->notify(database_file, undo_key);
    const std::string& row_idx = action_parameters0.empty() ? "" : action_parameters0[0];
    database->delete_table(table, row_idx);
    return nullptr;
}

Table* Read::execute(Database* database) {
    database->notify(database_file, undo_key);
    return database->read_table(table, action_parameters0, filter_where_parameters, filter_order_by_parameters);
}

Table* Update::execute(Database* database) {
    database->notify(database_file, undo_key);
    if (action_parameters0.size() < 3) return nullptr;

    size_t row_idx = 0;
    size_t col_idx = 0;
    auto [p1, ec1] = std::from_chars(action_parameters0[0].data(), action_parameters0[0].data() + action_parameters0[0].size(), row_idx);
    auto [p2, ec2] = std::from_chars(action_parameters0[1].data(), action_parameters0[1].data() + action_parameters0[1].size(), col_idx);

    if (ec1 == std::errc() && ec2 == std::errc()) {
        database->update_cell(table, row_idx, col_idx, action_parameters0[2]);
    }
    return nullptr;
}

Table* Union::execute(Database* database) {
    database->notify(database_file, undo_key);
    database->unionize_tables(table, action_parameters0);
    return nullptr;
}

Table* Load::execute(Database* database) {
    database->notify(database_file, undo_key);
    if (action_parameters0.size() >= 2) {
        Manager::get_instance()->get_database(database, action_parameters0[0], action_parameters0[1]);
    }
    return nullptr;
}

Table* Store::execute(Database* database) {
    if (action_parameters0.size() >= 2) {
        Manager::get_instance()->save_database(database, action_parameters0[0], action_parameters0[1]);
    }
    return nullptr;
}

Table* Undo::execute(Database* database) {
    Manager::get_instance()->get_database(database, database_file, undo_key);
    return nullptr;
}

QueryFactory* QueryFactory::get_instance() noexcept {
    static QueryFactory instance;
    return &instance;
}

std::unique_ptr<Query> QueryFactory::create_query(std::string_view type) const {
    if (type == "create") return std::make_unique<Create>();
    if (type == "insert") return std::make_unique<Insert>();
    if (type == "delete") return std::make_unique<Delete>();
    if (type == "read")   return std::make_unique<Read>();
    if (type == "update") return std::make_unique<Update>();
    if (type == "union")  return std::make_unique<Union>();
    if (type == "load")   return std::make_unique<Load>();
    if (type == "store")  return std::make_unique<Store>();
    if (type == "undo")   return std::make_unique<Undo>();
    return nullptr;
}