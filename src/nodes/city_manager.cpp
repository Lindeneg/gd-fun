#include "city_manager.h"

#include "../core/utils.h"
#include "./tile_manager.h"

godot::CL::CityManager::CityManager()
    : TilePlaceable<City>(PLACEABLE_CITY),
      entity_clicked_cb_(Callable(this, "handle_city_clicked_")) {}

godot::CL::CityManager::~CityManager() {}

void godot::CL::CityManager::handle_city_clicked_(StringName entry_name) {
    printf("CITY CLICKED\n");
    emit_signal("city_clicked", entry_name);
}

void godot::CL::CityManager::_enter_tree() {
    Node *node{get_node_or_null("../TileManager")};
    setup_tile_manager_(node);
}

void godot::CL::CityManager::_exit_tree() {
    Utils::disconnect(static_cast<TileManager *>(tile_manager_), "ready",
                      tile_manager_ready_cb_);
    tile_manager_ = nullptr;
}

void godot::CL::CityManager::iterate_children_(TypedArray<Entryable> nodes) {
    for (auto i = 0; i < nodes.size(); i++) {
        auto root{cast_to<City>(nodes[i])};
        handle_entryable_node_(root, root, nullptr);
        auto city_name = root->get_name();
        if (entries_.find(city_name) == entries_.end()) {
            entries_[city_name] = root;
            Utils::connect(root, City::SButtonClicked, entity_clicked_cb_);
        }
    }
}

godot::Array godot::CL::CityManager::get_cities_within_distance(City *from,
                                                                int distance) {
    auto onshore_from{from->get_entry_tile(TILE_ENTRY_ONSHORE)};
    auto offshore_from{from->get_entry_tile(TILE_ENTRY_OFFSHORE)};
    Array result{};
    for (auto city_el : entries_) {
        if (city_el.first == from->get_name()) {
            continue;
        }
        auto to{city_el.second};
        Dictionary city_result{};
        city_result["name"] = city_el.second->get_name();
        auto onshores{
            find_entry_path_(distance, onshore_from, to, TILE_ENTRY_ONSHORE)};
        auto offshores{
            find_entry_path_(distance, offshore_from, to, TILE_ENTRY_OFFSHORE)};
        if (onshores.size() > 0 || offshores.size() > 0) {
            city_result["onshores"] = onshores;
            city_result["offshores"] = offshores;
            result.push_back(city_result);
        }
    }
    return result;
}

void godot::CL::CityManager::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD("get_cities_within_distance", "from", "distance"),
        &CityManager::get_cities_within_distance);

    ClassDB::bind_method(D_METHOD("city_notify_tile_manager"),
                         &CityManager::notify_tile_manager);

    ClassDB::bind_method(D_METHOD("handle_city_clicked", "city_name"),
                         &CityManager::handle_city_clicked_);

    ClassDB::bind_method(D_METHOD("lock_city_buttons_except", "except"),
                         &CityManager::lock_buttons_except);
    ClassDB::bind_method(D_METHOD("unlock_city_buttons_except", "except"),
                         &CityManager::unlock_buttons_except);

    ClassDB::bind_method(D_METHOD("lock_all_city_buttons"),
                         &CityManager::lock_all_buttons);
    ClassDB::bind_method(D_METHOD("unlock_all_city_buttons"),
                         &CityManager::unlock_all_buttons);

    ClassDB::add_signal(
        "CityManager",
        MethodInfo("city_clicked", PropertyInfo(Variant::STRING_NAME, "name")));
}
