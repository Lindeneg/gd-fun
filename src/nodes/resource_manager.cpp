#include "resource_manager.h"

#include <godot_cpp/classes/marker2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "./tile_manager.h"

godot::CL::ResourceManager::ResourceManager()
    : TilePlaceable(PLACEABLE_RESOURCE), resources_({}) {}

godot::CL::ResourceManager::~ResourceManager() {}

void godot::CL::ResourceManager::iterate_children_(TypedArray<Node> nodes,
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
                auto *resource{static_cast<ResourceTile *>(parent)};
                Vector2 marker_position = node->get_position();
                Vector2i coords{tile_manager_->local_to_map(
                    parent->to_global(marker_position))};
                auto resource_entry_type =
                    static_cast<TileEntryType>(node->get_visibility_layer());
                resource->add_entry_point(coords, resource_entry_type);
                resources_[resource->get_name()] = resource;
            }
        } else {
            iterate_children_(grand_children, node);
        }
    }
}

godot::CL::ResourceTile *godot::CL::ResourceManager::get_resource(
    StringName name) const {
    auto resource = resources_.find(name);
    if (resource == resources_.end()) {
        return nullptr;
    }
    return resource->second;
}

void godot::CL::ResourceManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_resource", "name"),
                         &ResourceManager::get_resource);
}