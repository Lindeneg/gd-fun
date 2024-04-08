#include "city_manager.h"

#include <cstdio>
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

godot::CL::CityManager::CityManager()
    : TilePlaceable(PLACEABLE_CITY), cities_({}) {}

godot::CL::CityManager::~CityManager() {}

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
                    static_cast<CityEntryType>(node->get_visibility_layer());
                city->add_city_entry_point(coords, city_entry_type);
                cities_[city->get_name()] = city;
                printf("adding city: %s - %zu\n",
                       Utils::convert_gd_string(city->get_name()),
                       cities_.size());
            }
        } else {
            iterate_children_(grand_children, node);
        }
    }
}

godot::CL::City *godot::CL::CityManager::get_city(StringName name) const {
    auto city = cities_.find(name);
    if (city == cities_.end()) {
        return nullptr;
    }
    return city->second;
}

void godot::CL::CityManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_city", "name"), &CityManager::get_city);
}
