#ifndef CL_TRADING_TILE_MANAGER_H_
#define CL_TRADING_TILE_MANAGER_H_

#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/classes/wrapped.hpp>

#include "godot_cpp/core/binder_common.hpp"

namespace godot::CL {

enum TileLayer {
    TILE_BACKGROUND_LAYER = 0,
    TILE_VEGETATION_LAYER = 1,
    TILE_OBSTACLE_LAYER = 2
};

/* TileManager is meant to answer questions like:
 * 'Given these two points, give me back a path
 * between them that avoids (static) obstacles' */
class TileManager : public TileMap {
    GDCLASS(TileManager, TileMap)

   private:
    // TODO draw tiles with coordinates
    bool debug_mode_;
    Vector2i map_size_;

    void create_graph();

   protected:
    static void _bind_methods();

   public:
    TileManager();
    ~TileManager();

    void _ready() override;

    inline bool get_debug_mode() const { return debug_mode_; }
    inline void set_debug_mode(const bool m) { debug_mode_ = m; }
    inline Vector2i get_map_size() const { return map_size_; }
    void set_map_size(const Vector2i v);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::TileLayer);

#endif  // CL_TRADING_TILE_MANAGER_H_
