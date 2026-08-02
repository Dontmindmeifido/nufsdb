#include "version_control.h"
#include "../database/database.h"
#include "manager.h"

void Observer::subscribe(IObserver* observer) {
    if (observer && std::find(subscriptions.begin(), subscriptions.end(), observer) == subscriptions.end()) {
        subscriptions.push_back(observer);
    }
}

void Observer::unsubscribe(IObserver* observer) noexcept {
    subscriptions.erase(std::remove(subscriptions.begin(), subscriptions.end(), observer), subscriptions.end());
}

void Observer::notify(std::string_view database_file, std::string_view undo_key) {
    for (IObserver* observer : subscriptions) {
        observer->update(database_file, undo_key);
    }
}

VersionControl* VersionControl::get_instance() noexcept {
    static VersionControl instance;
    return &instance;
}

void VersionControl::update(std::string_view database_file, std::string_view undo_key) {
    Manager::get_instance()->save_database(Database::get_instance(), database_file, undo_key);
}