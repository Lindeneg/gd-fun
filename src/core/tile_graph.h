#ifndef CL_TRADING_TILE_GRAPH_H_
#define CL_TRADING_TILE_GRAPH_H_

#include <cstdint>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <utility>
#include <vector>

namespace godot {
class Vector2i;
}  // namespace godot

namespace godot::CL {
enum TileMat {
    TILE_MAT_NONE,
    TILE_MAT_GROUND,
    TILE_MAT_WATER,
    TILE_MAT_OBSTACLE
};

struct TileVertex {
    int32_t x;
    int32_t y;
    int weight;
    TileMat mat;
    TileVertex* previous;
    std::vector<std::pair<TileVertex*, int>> edges;

    TileVertex()
        : x(0),
          y(0),
          weight(0),
          mat(TILE_MAT_NONE),
          previous(nullptr),
          edges({}) {}
};

class TileGraph {
   private:
    std::vector<TileVertex*> vertices_;

   public:
    TileGraph();
    ~TileGraph();

    inline const std::vector<TileVertex*>& get_vertices_() const {
        return vertices_;
    }

    PackedVector2Array construct_path(const Vector2i start, const Vector2i end,
                                      const TileMat mat);
    void add_edge(TileVertex* v1, TileVertex* v2, const int weight);
    bool add_edge(const Vector2i v1, const Vector2i v2, const int weight);
    TileVertex* add_vertex(const Vector2i indicies, const int weight,
                           const TileMat mat, TileVertex* previous);
    TileVertex* get_vertex(const Vector2i indicies) const;
    void print() const;
    void destroy();
};

}  // namespace godot::CL

#endif  // CL_TRADING_TILE_GRAPH_H_
