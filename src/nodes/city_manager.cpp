#include "city_manager.h"

#include "../core/utils.h"

// SIGNALS
const char *godot::CL::CityManager::SCityClicked{"city_clicked"};

godot::CL::CityManager::CityManager()
    : TilePlaceable(PLACEABLE_CITY),
      cities_({}),
      city_clicked_cb_(Callable(this, "handle_city_clicked_")) {}

godot::CL::CityManager::~CityManager() {}

void godot::CL::CityManager::handle_city_clicked_(StringName city_name) {
    emit_signal(SCityClicked, city_name);
}

void godot::CL::CityManager::iterate_children_(TypedArray<Entryable> nodes) {
    for (auto i = 0; i < nodes.size(); i++) {
        auto root{cast_to<City>(nodes[i])};
        handle_entryable_node_(root, root, nullptr);
        auto city_name = root->get_name();
        if (cities_.find(city_name) == cities_.end()) {
            cities_[city_name] = root;
            Utils::connect(root, City::SButtonClicked, city_clicked_cb_);
        }
    }
}

void godot::CL::CityManager::lock_all_buttons() {
    for (auto city_el : cities_) {
        city_el.second->set_button_enabled(false);
    }
}

void godot::CL::CityManager::unlock_all_buttons() {
    for (auto city_el : cities_) {
        city_el.second->set_button_enabled(true);
    }
}

void godot::CL::CityManager::lock_buttons_except(
    TypedArray<StringName> except) {
    for (auto city_el : cities_) {
        city_el.second->set_button_enabled(except.has(city_el.first));
    }
}

void godot::CL::CityManager::unlock_buttons_except(
    TypedArray<StringName> except) {
    for (auto city_el : cities_) {
        city_el.second->set_button_enabled(!except.has(city_el.first));
    }
}

godot::CL::City *godot::CL::CityManager::get_city(StringName name) const {
    auto city = cities_.find(name);
    if (city == cities_.end()) {
        return nullptr;
    }
    return city->second;
}

godot::Array godot::CL::CityManager::get_cities_within_distance(City *from,
                                                                int distance) {
    return get_within_distance_(cities_, from, distance);
}

void godot::CL::CityManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_city", "name"), &CityManager::get_city);
    ClassDB::bind_method(
        D_METHOD("get_cities_within_distance", "from", "distance"),
        &CityManager::get_cities_within_distance);
    ClassDB::bind_method(D_METHOD("handle_city_clicked_", "city_name"),
                         &CityManager::handle_city_clicked_);

    ClassDB::bind_method(D_METHOD("lock_buttons_except", "except"),
                         &CityManager::lock_buttons_except);
    ClassDB::bind_method(D_METHOD("unlock_buttons_except", "except"),
                         &CityManager::unlock_buttons_except);

    ClassDB::bind_method(D_METHOD("lock_all_buttons"),
                         &CityManager::lock_all_buttons);
    ClassDB::bind_method(D_METHOD("unlock_all_buttons"),
                         &CityManager::unlock_all_buttons);

    ClassDB::add_signal(
        "CityManager",
        MethodInfo(SCityClicked,
                   PropertyInfo(Variant::STRING_NAME, "city_name")));
}
