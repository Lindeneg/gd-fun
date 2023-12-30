#include "tile_manager.h"

#include <iostream>

#include "godot_cpp/core/class_db.hpp"

godot::CL::TileManager::TileManager()
    : debug_mode_(false), map_size_(Vector2i()) {}

godot::CL::TileManager::~TileManager() {}

void godot::CL::TileManager::set_map_size(const Vector2i v) {
    map_size_ = v;
    create_graph();
}

void godot::CL::TileManager::create_graph() {
    for (int i = 0; i < map_size_.y; i++) {
        for (int j = 0; j < map_size_.x; j++) {
            std::cout << i << ", " << j << '\n';
        }
    }
}

void godot::CL::TileManager::_ready() {}

void godot::CL::TileManager::_bind_methods() {
    // MAP EDITOR PROPS
    ClassDB::bind_method(D_METHOD("get_map_size"), &TileManager::get_map_size);
    ClassDB::bind_method(D_METHOD("set_map_size", "v"),
                         &TileManager::set_map_size);

    ClassDB::add_property("TileManager",
                          PropertyInfo(Variant::VECTOR2I, "map_size"),
                          "set_map_size", "get_map_size");

    // DEBUG EDITOR PROPS
    ClassDB::bind_method(D_METHOD("get_debug_mode"),
                         &TileManager::get_debug_mode);
    ClassDB::bind_method(D_METHOD("set_debug_mode", "m"),
                         &TileManager::set_debug_mode);

    ClassDB::add_property_group("TileManager", "Debug", "");
    ClassDB::add_property("TileManager",
                          PropertyInfo(Variant::BOOL, "debug_mode"),
                          "set_debug_mode", "get_debug_mode");

    // BIND ENUMS
    BIND_ENUM_CONSTANT(TILE_BACKGROUND_LAYER);
    BIND_ENUM_CONSTANT(TILE_VEGETATION_LAYER);
    BIND_ENUM_CONSTANT(TILE_OBSTACLE_LAYER);
}
