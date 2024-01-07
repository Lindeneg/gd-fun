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

godot::CL::CityManager::CityManager()
    : cities_({}),
      tile_manager_(nullptr),
      tile_manager_ready_cb_(Callable(this, "notify_tile_manager_of_cities")) {}

godot::CL::CityManager::~CityManager() {}

void godot::CL::CityManager::setup_tile_manager_() {
    Node* node{get_node_or_null("../TileManager")};
    ERR_FAIL_NULL_EDMSG(node, "TileManager not found in tree");
    tile_manager_ = static_cast<TileManager*>(node);
    if (tile_manager_->is_node_ready()) {
        notify_tile_manager_of_cities();
    } else {
        tile_manager_->connect("ready", tile_manager_ready_cb_);
    }
}

void godot::CL::CityManager::iterate_children_(TypedArray<Node> nodes,
                                               Node2D* parent = nullptr) {
    auto size{nodes.size()};
    for (auto i = 0; i < size; i++) {
        auto* node{cast_to<Node2D>(nodes[i])};
        if (node == nullptr) {
            continue;
        }
        auto grand_children = node->get_children();
        if (grand_children.size() == 0 && parent != nullptr) {
            if (typeid(*node) == typeid(Sprite2D)) {
                handle_sprite_tile_manager_notification_(
                    static_cast<Sprite2D*>(node), parent);
            } else if (typeid(*node) == typeid(Marker2D)) {
                City* city = static_cast<City*>(parent);
                Vector2 marker_position = node->get_position();
                Vector2i coords{tile_manager_->local_to_map(
                    parent->to_global(marker_position))};
                auto city_entry_type =
                    static_cast<CityEntryType>(node->get_visibility_layer());
                city->add_city_entry_point(coords, city_entry_type);
                cities_[city->get_name()] = city;

            } else {
                WARN_PRINT_ED(
                    vformat("%s is unhandled city node", node->get_name()));
            }
        } else {
            iterate_children_(grand_children, node);
        }
    }
}

void godot::CL::CityManager::handle_sprite_tile_manager_notification_(
    Sprite2D* sprite, Node2D* parent) {
    // convert position to tile grid coordinates
    Vector2i coords{
        tile_manager_->local_to_map(parent->to_global(sprite->get_position()))};
    // the first tile is taken by default
    tile_manager_->update_vertex_mat(coords, 1);
    // now we need to figure out if we have more tiles to occupy
    int32_t tile_size{tile_manager_->get_tileset()->get_tile_size().x};
    Rect2 rect = sprite->get_region_rect();
    auto x_limit = int((rect.size.x / tile_size) - 1);
    auto y_limit = int((rect.size.y / tile_size) - 1);
    // update any offset tiles from origin
    for (auto x_offset = 0; x_offset < x_limit; x_offset++) {
        tile_manager_->update_vertex_mat(coords + Vector2i(x_offset + 1, 0), 1);
    }
    for (auto y_offset = 0; y_offset < y_limit; y_offset++) {
        tile_manager_->update_vertex_mat(coords + Vector2i(0, y_offset + 1), 1);
    }
}

void godot::CL::CityManager::notify_tile_manager_of_cities() {
    if (tile_manager_ == nullptr) {
        return;
    }
    if (Utils::is_in_editor()) {
        tile_manager_->reset_occupants();
    }
    auto children = get_children();
    iterate_children_(children);
    tile_manager_->set_rebuild_debug_graph(true);
}

void godot::CL::CityManager::_enter_tree() { setup_tile_manager_(); }

void godot::CL::CityManager::_exit_tree() {
    if (tile_manager_->is_connected("ready", tile_manager_ready_cb_)) {
        tile_manager_->disconnect("ready", tile_manager_ready_cb_);
    }
}

void godot::CL::CityManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_city", "name"), &CityManager::get_city);
    ClassDB::bind_method(D_METHOD("notify_tile_manager_of_cities"),
                         &CityManager::notify_tile_manager_of_cities);
}
