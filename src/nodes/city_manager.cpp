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

// TODO: rewrite this mess please
void godot::CL::CityManager::iterate_children_(TypedArray<Node> nodes,
                                               Node2D *parent = nullptr) {
    auto size{nodes.size()};
    for (auto i = 0; i < size; i++) {
        auto *node{cast_to<Node2D>(nodes[i])};
        if (node == nullptr) {
            continue;
        }
        auto grand_children = node->get_children();
        if (grand_children.size() == 0 && parent != nullptr) {
            if (typeid(*node) == typeid(Sprite2D)) {
                handle_sprite_tile_manager_notification_(
                    static_cast<Sprite2D *>(node), parent);
            } else if (typeid(*node) == typeid(Marker2D)) {
                auto *city{static_cast<City *>(parent)};
                Vector2 marker_position = node->get_position();
                Vector2i coords{tile_manager_->local_to_map(
                    parent->to_global(marker_position))};
                // TODO(5) Use an actual property:
                auto city_entry_type =
                    static_cast<TileEntryType>(node->get_visibility_layer());
                city->add_entry_point(coords, city_entry_type);
                auto city_name = city->get_name();
                if (cities_.find(city_name) == cities_.end()) {
                    cities_[city_name] = city;
                    Utils::connect(city, City::SButtonClicked,
                                   city_clicked_cb_);
                }
            }
        } else {
            iterate_children_(grand_children, node);
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
                return path;
            }
        }
    }
    return Array();
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
