#ifndef CL_TRADING_TILE_MANAGER_H_
#define CL_TRADING_TILE_MANAGER_H_

#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/tile_graph.h"

namespace godot {
class TileSet;
class Vector2i;
class PackedVector2Array;
}  // namespace godot

namespace godot::CL {

// DEPRECATED - do not use!!!
enum TileMat {
    TILE_MAT_NONE,
    TILE_MAT_GROUND,
    TILE_MAT_WATER,
    TILE_MAT_OBSTACLE
};

// tile layers
enum TileLayer {
    TILE_BACKGROUND_LAYER,
    TILE_RESOURCE_LAYER,
    TILE_OBSTACLE_LAYER,
    TILE_BRIDGE_LAYER
};

// custom tile data
enum TileDataLayer { TILE_DATA_LAYER_WEIGHT, TILE_DATA_LAYER_IS_WATER };
struct CellTileContext {
    TileSurface surface;
    int weight;
};

/* TileManager is meant to answer questions like:
 * 'Given these two points, on this material (onshore/offshore)
 * give me back a path between them that avoids (static) obstacles' */
class TileManager : public TileMap {
    GDCLASS(TileManager, TileMap)

   private:
    // draw colored tile grid
    bool debug_mode_;
    // rebuilds debug graph
    bool rebuild_debug_graph_;
    // payload to debug signal
    Array debug_array_;
    // tile size
    int32_t tile_size_;
    // map size
    Vector2i map_size_;
    // weighted tile graph
    TileGraph tile_graph_;

    // useful for tile_graph world navigation
    CellTileContext get_tile_context_(const Vector2i coords) const;
    // emits debug signal
    void emit_debug_signal_();
    // recreates debug array
    void set_debug_array_();
    // adds edge to tile
    void add_tile_edge_(const Vector2i coords, TileVertex* current);
    // creates weighted tile graph
    void create_graph_();
    // ensures required tile layers are created
    void ensure_layers_created_();
    // ensures tile_size_ member is set
    void ensure_tile_size_set_();
    // creates a named layer
    void create_layer_(const int32_t layer, const String name);
    void create_layer_(Ref<TileSet> tileset, const int32_t layer,
                       const String name, const Variant::Type type);

   protected:
    static void _bind_methods();

   public:
    TileManager();
    ~TileManager();

    void _ready() override;

    // constructs a path avoiding obstacles from start->end
    // respecting the given TileSurface constraint
    inline PackedVector2Array construct_path(const Vector2i start,
                                             const Vector2i end,
                                             const TileSurface surface) {
        return tile_graph_.astar_construct_path(start, end, surface);
    }
    Vector2i find_free_tile_on_surface(const Vector2i tile,
                                       const TileSurface surface,
                                       const Vector2 ignore_direction) const;
    void set_rebuild_debug_graph(const bool m);
    void set_debug_mode(const bool m);
    void set_map_size(const Vector2i v);
    void update_vertex_mat(const Vector2i v, const int m);

    inline void reset_occupants() { tile_graph_.reset_occupants(); }
    inline bool get_rebuild_debug_graph() const { return rebuild_debug_graph_; }
    inline bool get_debug_mode() const { return debug_mode_; }
    inline int32_t get_tile_graph_size() const {
        return tile_graph_.get_vertices_().size();
    }
    inline Vector2i get_map_size() const { return map_size_; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::TileLayer);
VARIANT_ENUM_CAST(godot::CL::TileDataLayer);
VARIANT_ENUM_CAST(godot::CL::TileMat);

#endif  // CL_TRADING_TILE_MANAGER_H_
