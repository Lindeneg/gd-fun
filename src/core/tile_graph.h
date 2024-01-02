#ifndef CL_TRADING_TILE_GRAPH_H_
#define CL_TRADING_TILE_GRAPH_H_

#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <map>
#include <vector>

namespace godot::CL {

enum TileMat {
    TILE_MAT_NONE,
    TILE_MAT_GROUND,
    TILE_MAT_WATER,
    TILE_MAT_OBSTACLE
};

struct TileVertex {
    Vector2i coords;
    int weight;
    TileMat mat;
    TileVertex* previous;
    std::vector<TileVertex*> edges;

    TileVertex()
        : coords(Vector2i()),
          weight(0),
          mat(TILE_MAT_NONE),
          previous(nullptr),
          edges({}) {}
};

// int with default value of max instead of 0
struct AStarScoreNode {
    int val;
    // TODO remove hardcoded value
    AStarScoreNode() : val(5e4) {}
    AStarScoreNode(int i) : val(i) {}
};

// keep track of where we came from, so path between
// start/end can be reconstructed once end is reached
using AStarCameFromMap = std::map<TileVertex*, TileVertex*>;
// best currently known paths from start to node N
using AStarGScoreMap = std::map<TileVertex*, AStarScoreNode>;
// best guess as to how cheap a path from start to end
// could be if it goes through node N
using AStarFScoreMap = std::map<TileVertex*, AStarScoreNode>;
// prio queue doesn't contain a method to see if it contains an element
// so, because I'm stupid, I have another map that keeps track of that
// if value is 0, not added, if value is 1, then it is added
using AStarPrioQueueMember = std::map<TileVertex*, int>;

class TileGraph {
   private:
    static const int MaxPathLength_;

    std::vector<TileVertex*> vertices_;

    int astar_calculate_heuristic_(TileVertex* current, TileVertex* goal) const;
    PackedVector2Array astar_reconstruct_path_(const Vector2i start,
                                               AStarCameFromMap came_from,
                                               TileVertex* current);

   public:
    TileGraph();
    ~TileGraph();

    inline const std::vector<TileVertex*>& get_vertices_() const {
        return vertices_;
    }

    PackedVector2Array astar_construct_path(TileVertex* start, TileVertex* end,
                                            const TileMat mat);
    PackedVector2Array astar_construct_path(Vector2i start, Vector2i end,
                                            const TileMat mat);
    void add_edge(TileVertex* v1, TileVertex* v2);
    bool add_edge(const Vector2i v1, const Vector2i v2);
    TileVertex* add_vertex(const Vector2i indicies, const int weight,
                           const TileMat mat, TileVertex* previous);
    TileVertex* get_vertex(const Vector2i indicies) const;
    void print() const;
    void destroy();
};

}  // namespace godot::CL

#endif  // CL_TRADING_TILE_GRAPH_H_
