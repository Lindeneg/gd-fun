#include "tile_placeable.h"

#include <godot_cpp/classes/marker2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../nodes/city.h"
#include "../nodes/resource.h"
#include "../nodes/tile_manager.h"
#include "./utils.h"

template <typename TNode>
godot::CL::TilePlaceable<TNode>::TilePlaceable()
    : godot::CL::TilePlaceable<TNode>(PLACEABLE_CITY) {}

template <typename TNode>
godot::CL::TilePlaceable<TNode>::TilePlaceable(
    godot::CL::TilePlaceableKind kind)
    : kind_(kind),
      entries_({}),
      tile_manager_(nullptr),
      tile_manager_ready_cb_(Callable(this, "notify_tile_manager")) {}

template <typename TNode>
godot::CL::TilePlaceable<TNode>::~TilePlaceable() {}

template <typename TNode>
godot::TypedArray<godot::Vector2>
godot::CL::TilePlaceable<TNode>::get_local_path_(PackedVector2Array path) {
    TypedArray<Vector2> result{};
    auto size{path.size()};
    for (int i = 0; i < size; i++) {
        result.append(tile_manager_->map_to_local(path[i]));
    }
    return result;
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::handle_entryable_node_(Entryable *root,
                                                             Node *node,
                                                             Node2D *parent) {
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

template <typename TNode>
godot::Array godot::CL::TilePlaceable<TNode>::find_entry_path_(
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

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::setup_tile_manager_(Node *node) {
    ERR_FAIL_NULL_EDMSG(node, "TileManager not found in tree");
    tile_manager_ = static_cast<TileManager *>(node);
    if (tile_manager_->is_node_ready()) {
        notify_tile_manager();
    } else {
        Utils::connect(tile_manager_, "ready", tile_manager_ready_cb_);
    }
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::handle_sprite_tile_manager_notification_(
    Sprite2D *sprite, Node2D *parent) {
    // convert position to tile grid coordinates
    Vector2i tile_pos{
        tile_manager_->local_to_map(parent->to_global(sprite->get_position()))};
    // the first tile is taken by default
    tile_manager_->add_occupant(tile_pos, kind_);
    // now we need to figure out if we have more tiles to occupy
    int32_t tile_size{tile_manager_->get_tileset()->get_tile_size().x};
    Rect2 rect = sprite->get_region_rect();
    auto x_limit = int((rect.size.x / tile_size) - 1);
    auto y_limit = int((rect.size.y / tile_size) - 1);
    // update any offset tiles from origin
    for (auto x_offset = 0; x_offset < x_limit; x_offset++) {
        tile_manager_->add_occupant(tile_pos + Vector2i(x_offset + 1, 0),
                                    kind_);
    }
    for (auto y_offset = 0; y_offset < y_limit; y_offset++) {
        tile_manager_->add_occupant(tile_pos + Vector2i(0, y_offset + 1),
                                    kind_);
    }
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::lock_all_buttons() {
    for (auto entry : entries_) {
        entry.second->set_button_enabled(false);
    }
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::unlock_all_buttons() {
    for (auto entry : entries_) {
        entry.second->set_button_enabled(true);
    }
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::lock_buttons_except(
    TypedArray<StringName> except) {
    for (auto entry : entries_) {
        entry.second->set_button_enabled(except.has(entry.first));
    }
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::unlock_buttons_except(
    TypedArray<StringName> except) {
    for (auto entry : entries_) {
        entry.second->set_button_enabled(!except.has(entry.first));
    }
}

template <typename TNode>
void godot::CL::TilePlaceable<TNode>::notify_tile_manager() {
    if (tile_manager_ == nullptr) {
        return;
    }
    if (Utils::is_in_editor()) {
        tile_manager_->reset_occupants_kind(kind_);
    }
    iterate_children_(get_children());
}

template class godot::CL::TilePlaceable<godot::CL::Entryable>;
template class godot::CL::TilePlaceable<godot::CL::City>;
template class godot::CL::TilePlaceable<godot::CL::ResourceTile>;
