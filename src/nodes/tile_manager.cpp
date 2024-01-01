#include "tile_manager.h"

#include <cstdint>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/tile_data.hpp>
#include <godot_cpp/classes/tile_set.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include "../core/tile_graph.h"
#include "../core/utils.h"

godot::CL::TileManager::TileManager()
    : debug_mode_(false),
      rebuild_debug_graph_(false),
      tile_size_(0),
      debug_array_(Array()),
      map_size_(Vector2i()),
      tile_graph_(TileGraph()) {}

godot::CL::TileManager::~TileManager() {}

void godot::CL::TileManager::set_map_size(const Vector2i v) { map_size_ = v; }

void godot::CL::TileManager::set_debug_mode(const bool m) {
    debug_mode_ = m;
    emit_debug_signal_();
}

void godot::CL::TileManager::set_rebuild_debug_graph(const bool m) {
    if (!debug_mode_) {
        return;
    }
    rebuild_debug_graph_ = true;
    debug_array_.clear();
    create_graph_();
    emit_debug_signal_();
    rebuild_debug_graph_ = false;
}

void godot::CL::TileManager::emit_debug_signal_() {
    if (debug_mode_) {
        set_debug_array_();
        emit_signal("draw_debug_grid", tile_size_, debug_array_);
    } else {
        emit_signal("remove_debug_grid");
    }
}

void godot::CL::TileManager::set_debug_array_() {
    const auto size{get_tile_graph_size()};
    if (size > debug_array_.size()) {
        debug_array_.resize(size);
    }
    const auto& vertices{tile_graph_.get_vertices_()};
    const auto vertices_size = vertices.size();
    for (int32_t i = 0; i < vertices_size; i++) {
        const auto& vertex{vertices[i]};
        auto dict{Dictionary()};
        dict["coords"] = map_to_local(Vector2i(vertex->x, vertex->y));
        dict["mat"] = int(vertex->mat);
        debug_array_.insert(i, dict);
    }
}

void godot::CL::TileManager::create_graph_() {
    tile_graph_.destroy();
    for (int i = 0; i < map_size_.y; i++) {
        TileVertex* previous{nullptr};
        for (int j = 0; j < map_size_.x; j++) {
            const auto coords{Vector2i(j, i)};
            const CellTileContext tile_context{get_tile_context_(coords)};
            TileVertex* new_vertex{tile_graph_.add_vertex(
                coords, tile_context.weight, tile_context.mat, previous)};
            previous = new_vertex;
            if (i > 0) {
                add_tile_edge_(Vector2i(j, i - 1), new_vertex);
            }
            if (j > 0) {
                add_tile_edge_(Vector2i(j - 1, i), new_vertex);
            }
        }
    }
    if (debug_mode_) {
        std::cout << "\n\nGRAPH: \n";
        tile_graph_.print();
    }
}

void godot::CL::TileManager::add_tile_edge_(const Vector2i coords,
                                            TileVertex* current) {
    TileVertex* vertex{tile_graph_.get_vertex(coords)};
    if (vertex != nullptr) {
        const auto weight{int((current->weight + vertex->weight / 2))};
        tile_graph_.add_edge(current, vertex, weight);
    }
}

godot::CL::CellTileContext godot::CL::TileManager::get_tile_context_(
    const Vector2i coords) const {
    auto result{CellTileContext()};
    TileData* obs_data{get_cell_tile_data(TILE_OBSTACLE_LAYER, coords)};
    if (obs_data != nullptr) {
        result.mat = TILE_MAT_OBSTACLE;
        return result;
    }
    TileData* bg_data{get_cell_tile_data(TILE_BACKGROUND_LAYER, coords)};
    if (bg_data != nullptr) {
        const auto weight{
            static_cast<float>(bg_data->get_custom_data("weight"))};
        const auto is_water{
            static_cast<bool>(bg_data->get_custom_data("is_water"))};
        result.weight = weight;
        if (is_water) {
            result.mat = TILE_MAT_WATER;
            return result;
        }
        result.mat = TILE_MAT_GROUND;
    }
    return result;
}

void godot::CL::TileManager::ensure_layers_created_() {
    if (get_layers_count() == 1) {
        remove_layer(0);
        create_layer_(TILE_BACKGROUND_LAYER, "Background");
        create_layer_(TILE_VEGETATION_LAYER, "Vegetation");
        create_layer_(TILE_OBSTACLE_LAYER, "Obstacle");
    }
    Ref<TileSet> tileset{get_tileset()};
    if (tileset != nullptr && tileset->get_custom_data_layers_count() == 0) {
        create_layer_(tileset, TILE_DATA_LAYER_WEIGHT, "weight", Variant::INT);
        create_layer_(tileset, TILE_DATA_LAYER_IS_WATER, "is_water",
                      Variant::BOOL);
    }
}

void godot::CL::TileManager::ensure_tile_size_set_() {
    Ref<TileSet> tileset{get_tileset()};
    if (tileset == nullptr) {
        tile_size_ = 0;
        return;
    }
    // only squares are utilized
    tile_size_ = tileset->get_tile_size().x;
}

void godot::CL::TileManager::create_layer_(Ref<TileSet> tileset,
                                           const int32_t layer,
                                           const String name,
                                           const Variant::Type type) {
    if (tileset == nullptr) {
        return;
    }
    tileset->add_custom_data_layer(layer);
    tileset->set_custom_data_layer_name(layer, name);
    tileset->set_custom_data_layer_type(layer, type);
}

void godot::CL::TileManager::create_layer_(const int32_t layer,
                                           const String name) {
    add_layer(layer);
    set_layer_name(layer, name);
    set_layer_y_sort_enabled(layer, true);
}

void godot::CL::TileManager::_ready() {
    if (::CL::is_in_editor()) {
        set_y_sort_enabled(true);
        ensure_layers_created_();
    }
    ensure_tile_size_set_();
    create_graph_();
    emit_debug_signal_();
}

void godot::CL::TileManager::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("construct_path", "start", "end", "mat"),
                         &TileManager::construct_path);

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
    ClassDB::bind_method(D_METHOD("get_rebuild_debug_graph"),
                         &TileManager::get_rebuild_debug_graph);
    ClassDB::bind_method(D_METHOD("set_rebuild_debug_graph", "m"),
                         &TileManager::set_rebuild_debug_graph);

    ClassDB::add_property_group("TileManager", "Debug", "");
    ClassDB::add_property("TileManager",
                          PropertyInfo(Variant::BOOL, "debug_mode"),
                          "set_debug_mode", "get_debug_mode");
    ClassDB::add_property("TileManager",
                          PropertyInfo(Variant::BOOL, "rebuild_debug_graph"),
                          "set_rebuild_debug_graph", "get_rebuild_debug_graph");

    // SIGNALS
    ClassDB::add_signal(
        "TileManager",
        MethodInfo("draw_debug_grid", PropertyInfo(Variant::INT, "size"),
                   PropertyInfo(Variant::ARRAY, "tiles")));
    ClassDB::add_signal("TileManager", MethodInfo("remove_debug_grid"));

    // BIND ENUMS
    BIND_ENUM_CONSTANT(TILE_BACKGROUND_LAYER);
    BIND_ENUM_CONSTANT(TILE_VEGETATION_LAYER);
    BIND_ENUM_CONSTANT(TILE_OBSTACLE_LAYER);

    BIND_ENUM_CONSTANT(TILE_DATA_LAYER_WEIGHT);
    BIND_ENUM_CONSTANT(TILE_DATA_LAYER_IS_WATER);

    BIND_ENUM_CONSTANT(TILE_MAT_NONE);
    BIND_ENUM_CONSTANT(TILE_MAT_GROUND);
    BIND_ENUM_CONSTANT(TILE_MAT_WATER);
    BIND_ENUM_CONSTANT(TILE_MAT_OBSTACLE);
}