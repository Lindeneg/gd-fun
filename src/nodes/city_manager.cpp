#include "city_manager.h"

#include <godot_cpp/variant/dictionary.hpp>

#include "../core/utils.h"

// SIGNALS
const char *godot::CL::CityManager::SCityClicked{"city_clicked"};

godot::CL::CityManager::CityManager()
    : TilePlaceable(PLACEABLE_CITY),
      city_clicked_cb_(Callable(this, "handle_city_clicked_")) {}

godot::CL::CityManager::~CityManager() {}

void godot::CL::CityManager::handle_city_clicked_(StringName city_name) {
    emit_signal(SCityClicked, city_name);
}

void godot::CL::CityManager::iterate_children_(TypedArray<Entryable> nodes) {
    for (int i = 0; i < nodes.size(); i++) {
        City *root{cast_to<City>(nodes[i])};
        handle_entryable_node_(root, root, nullptr);
        StringName city_name = root->get_name();
        if (!entries_.has(city_name)) {
            entries_[city_name] = root;
            Utils::connect(root, City::SButtonClicked, city_clicked_cb_);
        }
    }
}

godot::CL::City *godot::CL::CityManager::get_city(StringName name) const {
    auto city{cast_to<City>(entries_[name])};
    if (!city) {
        return nullptr;
    }
    return city;
}

void godot::CL::CityManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_city", "name"), &CityManager::get_city);
    ClassDB::bind_method(D_METHOD("get_cities"), &CityManager::get_cities);
    ClassDB::bind_method(
        D_METHOD("get_cities_within_distance", "from", "distance"),
        &CityManager::get_cities_within_distance);
    ClassDB::bind_method(D_METHOD("handle_city_clicked_", "city_name"),
                         &CityManager::handle_city_clicked_);

    ClassDB::add_signal(
        "CityManager",
        MethodInfo(SCityClicked,
                   PropertyInfo(Variant::STRING_NAME, "city_name")));
}
