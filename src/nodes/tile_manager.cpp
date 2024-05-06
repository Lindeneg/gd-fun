#include "tile_manager.h"

#include <cstdint>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/tile_data.hpp>
#include <godot_cpp/classes/tile_set.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>

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

void godot::CL::TileManager::set_map_size(const Vector2i v) {
    map_size_ = v;
    create_graph_();
}

void godot::CL::TileManager::set_debug_mode(const bool m) {
    debug_mode_ = m;
    tile_graph_.set_debug(m);
    emit_debug_signal_();
}

// used inside editor as a lazy button-like action
void godot::CL::TileManager::set_rebuild_debug_graph(const bool m) {
#ifdef CL_TRADING_DEBUG
    if (!debug_mode_) {
        return;
    }
    debug_array_.clear();
    create_graph_();
#endif
}

void godot::CL::TileManager::emit_debug_signal_() {
#ifdef CL_TRADING_DEBUG
    if (debug_mode_ && get_tile_graph_size() > 0 && tile_size_ > 0) {
        set_debug_array_();
        emit_signal("draw_debug_grid", tile_size_, debug_array_);
    } else {
        emit_signal("remove_debug_grid");
    }
#endif
}

void godot::CL::TileManager::set_debug_array_() {
#ifdef CL_TRADING_DEBUG
    const auto size{get_tile_graph_size()};
    if (size > debug_array_.size()) {
        debug_array_.resize(size);
    }
    const auto &vertices{tile_graph_.get_vertices_()};
    const auto vertices_size = vertices.size();
    for (int32_t i = 0; i < vertices_size; i++) {
        const auto &vertex{vertices[i]};
        auto dict{Dictionary()};
        dict["coords"] = map_to_local(vertex->coords);
        dict["surface"] = vertex->surface;
        debug_array_.insert(i, dict);
    }
#endif
}

void godot::CL::TileManager::create_graph_() {
    tile_graph_.destroy();
    for (int i = 0; i < map_size_.y; i++) {
        for (int j = 0; j < map_size_.x; j++) {
            const auto coords{Vector2i(j, i)};
            const CellTileContext tile_context{get_tile_context_(coords)};
            TileVertex *new_vertex{tile_graph_.add_vertex(
                coords, tile_context.weight, tile_context.surface)};
            // add up neighbor
            if (i > 0) {
                add_tile_edge_(Vector2i(j, i - 1), new_vertex);
            }
            // add left neighbor
            if (j > 0) {
                add_tile_edge_(Vector2i(j - 1, i), new_vertex);
            }
        }
    }
#ifdef CL_TRADING_DEBUG
    emit_debug_signal_();
#endif
}

void godot::CL::TileManager::add_tile_edge_(const Vector2i coords,
                                            TileVertex *current) {
    TileVertex *vertex{tile_graph_.get_vertex(coords)};
    ERR_FAIL_NULL_EDMSG(vertex, vformat("vertex (%d,%d) was not found in graph",
                                        coords.x, coords.y));
    tile_graph_.add_edge(current, vertex);
}

void godot::CL::TileManager::add_occupant(const Vector2i v, const int k) {
    tile_graph_.add_foreign_occupant(v, k);
}

void godot::CL::TileManager::remove_occupant(const Vector2i v) {
    tile_graph_.remove_foreign_occupant(v);
}

godot::CL::CellTileContext godot::CL::TileManager::get_tile_context_(
    const Vector2i coords) const {
    auto result{CellTileContext()};
    if (tile_graph_.has_occupant(coords)) {
        result.surface = TILE_SURFACE_OBSTACLE;
        return result;
    }
    TileData *bridge_data{get_cell_tile_data(TILE_BRIDGE_LAYER, coords)};
    if (bridge_data != nullptr) {
        result.surface = TILE_SURFACE_BRIDGE;
        return result;
    }
    TileData *obs_data{get_cell_tile_data(TILE_OBSTACLE_LAYER, coords)};
    if (obs_data != nullptr) {
        result.surface = TILE_SURFACE_OBSTACLE;
        return result;
    }
    TileData *bg_data{get_cell_tile_data(TILE_BACKGROUND_LAYER, coords)};
    if (bg_data != nullptr) {
        const auto weight{
            static_cast<float>(bg_data->get_custom_data("weight"))};
        const auto is_water{
            static_cast<bool>(bg_data->get_custom_data("is_water"))};
        result.weight = weight;
        if (is_water) {
            result.surface = TILE_SURFACE_WATER;
            return result;
        }
        result.surface = TILE_SURFACE_GROUND;
    }
    return result;
}

void godot::CL::TileManager::ensure_layers_created_() {
    if (get_layers_count() == 1) {
        remove_layer(0);
        create_layer_(TILE_BACKGROUND_LAYER, "Background");
        create_layer_(TILE_RESOURCE_LAYER, "Resource");
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
    if (Utils::is_in_editor()) {
        set_y_sort_enabled(true);
        ensure_layers_created_();
    }
    ensure_tile_size_set_();
    create_graph_();
}

void godot::CL::TileManager::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("construct_path", "start", "end", "mat"),
                         &TileManager::construct_path);
    ClassDB::bind_method(D_METHOD("reset_occupants"),
                         &TileManager::reset_occupants);
    ClassDB::bind_method(D_METHOD("reset_occupants_kind", "k"),
                         &TileManager::reset_occupants_kind);

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

    // SIGNALS
    ClassDB::add_signal(
        "TileManager",
        MethodInfo("draw_debug_grid", PropertyInfo(Variant::INT, "size"),
                   PropertyInfo(Variant::ARRAY, "tiles")));
    ClassDB::add_signal("TileManager", MethodInfo("remove_debug_grid"));

    // BIND ENUMS
    BIND_ENUM_CONSTANT(TILE_BACKGROUND_LAYER);
    BIND_ENUM_CONSTANT(TILE_RESOURCE_LAYER);
    BIND_ENUM_CONSTANT(TILE_OBSTACLE_LAYER);
    BIND_ENUM_CONSTANT(TILE_BRIDGE_LAYER);

    BIND_ENUM_CONSTANT(TILE_DATA_LAYER_WEIGHT);
    BIND_ENUM_CONSTANT(TILE_DATA_LAYER_IS_WATER);
}
