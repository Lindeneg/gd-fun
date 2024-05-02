#ifndef CL_TRADING_TILE_GRAPH_H_
#define CL_TRADING_TILE_GRAPH_H_

#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <map>
#include <vector>

#include "../core/utils.h"

namespace godot::CL {

struct ForeignOccupant {
    int surface;
    int kind;
};

struct TileVertex {
    Vector2i coords;
    int weight;
    TileSurface surface;
    std::vector<TileVertex *> edges;

    TileVertex()
        : coords(Vector2i()),
          weight(0),
          surface(TILE_SURFACE_NONE),
          edges({}) {}
};

// int with an arbitrary default
// value of 50000 instead of 0
struct AStarScoreNode {
    int val;
    AStarScoreNode() : val(5e4) {}
    AStarScoreNode(int i) : val(i) {}
};

using AStarCameFromMap = std::map<TileVertex *, TileVertex *>;
using AStarGScoreMap = std::map<TileVertex *, AStarScoreNode>;
using AStarFScoreMap = std::map<TileVertex *, AStarScoreNode>;
using AStarPrioQueueMember = std::map<TileVertex *, int>;

/* TileGraph is a weighted undirected graph
 * structure that is used for pathfinding */
class TileGraph {
   private:
    static const int MaxPathLength_;

    std::map<const Vector2i, ForeignOccupant> foreign_occupants_;
    std::vector<TileVertex *> vertices_;

    int astar_calculate_heuristic_(TileVertex *current, TileVertex *goal) const;
    PackedVector2Array astar_reconstruct_path_(const Vector2i start,
                                               AStarCameFromMap came_from,
                                               TileVertex *current);

   public:
    TileGraph();
    ~TileGraph();

    inline const std::vector<TileVertex *> &get_vertices_() const {
        return vertices_;
    }
    inline void reset_occupants() { foreign_occupants_.clear(); }
    inline bool has_occupant(const Vector2i v) const {
        auto occupant{foreign_occupants_.find(v)};
        if (occupant != foreign_occupants_.end() &&
            occupant->second.surface > 0) {
            return true;
        }
        return false;
    }
    inline bool has_occupant_kind(const Vector2i v, const int k) const {
        auto occupant{foreign_occupants_.find(v)};
        if (occupant != foreign_occupants_.end() &&
            occupant->second.surface > 0 && occupant->second.kind == k) {
            return true;
        }
        return false;
    }

    PackedVector2Array astar_construct_path(TileVertex *start, TileVertex *end,
                                            const TileSurface surface);
    PackedVector2Array astar_construct_path(Vector2i start, Vector2i end,
                                            const TileSurface surface);
    void add_edge(TileVertex *v1, TileVertex *v2);
    void add_edge(const Vector2i v1, const Vector2i v2);
    void reset_occupants_kind(const int kind);
    void add_foreign_occupant(const Vector2i v, const int kind);
    void remove_foreign_occupant(const Vector2i v);
    TileVertex *add_vertex(const Vector2i indicies, const int weight,
                           const TileSurface surface);
    TileVertex *get_vertex(const Vector2i indicies) const;
    void print() const;
    void destroy();
};

}  // namespace godot::CL

#endif  // CL_TRADING_TILE_GRAPH_H_
