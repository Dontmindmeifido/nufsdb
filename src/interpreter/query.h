#pragma once
#include <string>
#include <vector>
#include <memory>
#include <string_view>
#include "../database/database.h"
#include "../disk_storage/manager.h"

struct Query {
    std::string action;
    std::vector<std::string> action_parameters0;
    std::vector<std::string> action_parameters1;
    std::string table;
    std::vector<std::string> filter_where_parameters;
    std::vector<std::string> filter_order_by_parameters;
    std::string database_file{".database.version_control"};
    std::string undo_key{"undo_key"};

    virtual ~Query() = default;
    virtual Table* execute(Database* database) = 0;
};

struct Create final : public Query {
    Table* execute(Database* database) override;
};

struct Read final : public Query {
    Table* execute(Database* database) override;
};

struct Insert final : public Query {
    Table* execute(Database* database) override;
};

struct Delete final : public Query {
    Table* execute(Database* database) override;
};

struct Update final : public Query {
    Table* execute(Database* database) override;
};

struct Union final : public Query {
    Table* execute(Database* database) override;
};

struct Load final : public Query {
    Table* execute(Database* database) override;
};

struct Store final : public Query {
    Table* execute(Database* database) override;
};

struct Undo final : public Query {
    Table* execute(Database* database) override;
};

class QueryFactory {
    QueryFactory() = default;

public:
    static QueryFactory* get_instance() noexcept;
    std::unique_ptr<Query> create_query(std::string_view type) const;
};