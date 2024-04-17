#include "tile_placeable.h"

#include "../nodes/tile_manager.h"
#include "./utils.h"

godot::CL::TilePlaceable::TilePlaceable()
    : godot::CL::TilePlaceable(PLACEABLE_CITY) {}

godot::CL::TilePlaceable::TilePlaceable(godot::CL::TilePlaceableKind kind)
    : kind_(kind),
      tile_manager_(nullptr),
      tile_manager_ready_cb_(Callable(this, "notify_tile_manager")) {}

godot::CL::TilePlaceable::~TilePlaceable() {}

void godot::CL::TilePlaceable::_enter_tree() { setup_tile_manager_(); }

void godot::CL::TilePlaceable::_exit_tree() {
    Utils::disconnect(tile_manager_, "ready", tile_manager_ready_cb_);
    tile_manager_ = nullptr;
}

void godot::CL::TilePlaceable::setup_tile_manager_() {
    Node *node{get_node_or_null("../TileManager")};
    ERR_FAIL_NULL_EDMSG(node, "TileManager not found in tree");
    tile_manager_ = static_cast<TileManager *>(node);
    if (tile_manager_->is_node_ready()) {
        notify_tile_manager();
    } else {
        Utils::connect(tile_manager_, "ready", tile_manager_ready_cb_);
    }
}

void godot::CL::TilePlaceable::handle_sprite_tile_manager_notification_(
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

void godot::CL::TilePlaceable::notify_tile_manager() {
    if (tile_manager_ == nullptr) {
        return;
    }
    if (Utils::is_in_editor()) {
        tile_manager_->reset_occupants_kind(kind_);
    }
    iterate_children_(get_children());
}

void godot::CL::TilePlaceable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("notify_tile_manager"),
                         &TilePlaceable::notify_tile_manager);
}
