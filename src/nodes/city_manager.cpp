#include "city_manager.h"

#include <godot_cpp/classes/marker2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../core/utils.h"
#include "./tile_manager.h"

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

void godot::CL::CityManager::handle_entryable_node_(Entryable *root, Node *node,
                                                    Node2D *parent = nullptr) {
    auto children{node->get_children()};
    if (children.size() == 0 && parent != nullptr) {
        if (typeid(*node) == typeid(Sprite2D)) {
            handle_sprite_tile_manager_notification_(
                static_cast<Sprite2D *>(node), parent);
        } else if (typeid(*node) == typeid(Marker2D)) {
            auto marker{static_cast<Marker2D *>(node)};
            Vector2 marker_position = marker->get_position();
            Vector2i coords{tile_manager_->local_to_map(
                parent->to_global(marker_position))};
            auto city_entry_type =
                static_cast<TileEntryType>(marker->get_visibility_layer());
            root->add_entry_point(coords, city_entry_type);
        }
    } else {
        for (int i = 0; i < children.size(); i++) {
            handle_entryable_node_(root, cast_to<Node>(children[i]),
                                   cast_to<Node2D>(node));
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
    auto onshore_from{from->get_entry_tile(TILE_ENTRY_ONSHORE)};
    auto offshore_from{from->get_entry_tile(TILE_ENTRY_OFFSHORE)};
    Array result{};
    for (auto city_el : cities_) {
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

godot::Array godot::CL::CityManager::find_entry_path_(
    const int max_distance, const Dictionary from, const Entryable *to_entry,
    const TileEntryType entry_type) {
    if (from["found"]) {
        auto to{to_entry->get_entry_tile(entry_type)};
        if (to["found"]) {
            auto surface{TILE_SURFACE_GROUND};
            if (entry_type == TILE_ENTRY_OFFSHORE) {
                surface = TILE_SURFACE_WATER;
            }
            auto path{tile_manager_->construct_path(
                static_cast<Vector2i>(from["coords"]),
                static_cast<Vector2i>(to["coords"]), surface)};
            auto path_size{path.size()};
            if (path_size > 0 && path_size <= max_distance) {
                return get_local_path_(path);
            }
        }
    }
    return Array();
}

godot::TypedArray<godot::Vector2> godot::CL::CityManager::get_local_path_(
    PackedVector2Array path) {
    TypedArray<Vector2> result{};
    auto size = path.size();
    for (int i = 0; i < size; i++) {
        result.append(tile_manager_->map_to_local(path[i]));
    }
    return result;
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
