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

enum TileLayer {
    TILE_BACKGROUND_LAYER = 0,
    TILE_VEGETATION_LAYER = 1,
    TILE_OBSTACLE_LAYER = 2
};

enum TileDataLayer { TILE_DATA_LAYER_WEIGHT = 0, TILE_DATA_LAYER_IS_WATER = 1 };

struct CellTileContext {
    TileMat mat;
    int weight;
};

/* TileManager is meant to answer questions like:
 * 'Given these two points, give me back a path
 * between them that avoids (static) obstacles' */
class TileManager : public TileMap {
    GDCLASS(TileManager, TileMap)

   private:
    bool debug_mode_;
    bool rebuild_debug_graph_;
    int32_t tile_size_;
    Array debug_array_;
    Vector2i map_size_;
    TileGraph tile_graph_;

    CellTileContext get_tile_context_(const Vector2i coords) const;
    void emit_debug_signal_();
    void set_debug_array_();
    void add_tile_edge_(const Vector2i coords, TileVertex* current);
    void create_graph_();
    void ensure_layers_created_();
    void ensure_tile_size_set_();
    void create_layer_(const int32_t layer, const String name);
    void create_layer_(Ref<TileSet> tileset, const int32_t layer,
                       const String name, const Variant::Type type);

   protected:
    static void _bind_methods();

   public:
    TileManager();
    ~TileManager();

    void _ready() override;

    inline PackedVector2Array construct_path(const Vector2i start,
                                             const Vector2i end,
                                             const TileMat mat) {
        return tile_graph_.construct_path(start, end, mat);
    }
    void set_rebuild_debug_graph(const bool m);
    void set_debug_mode(const bool m);
    void set_map_size(const Vector2i v);

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
