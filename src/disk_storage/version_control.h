#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(std::string_view database_file, std::string_view undo_key) = 0;
};

class Observer {
    std::vector<IObserver*> subscriptions;

public:
    void subscribe(IObserver* observer);
    void unsubscribe(IObserver* observer) noexcept;
    void notify(std::string_view database_file, std::string_view undo_key);
};

class VersionControl : public IObserver {
    VersionControl() = default;

public:
    static VersionControl* get_instance() noexcept;
    void update(std::string_view database_file, std::string_view undo_key) override;
};