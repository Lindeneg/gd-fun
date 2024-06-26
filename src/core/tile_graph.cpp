#include "tile_graph.h"

#include <cmath>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <queue>
#include <vector>

#ifdef CL_TRADING_DEBUG
MAKE_M_LOG(TILELOG, TileGraph)
#endif

const int godot::CL::TileGraph::MaxPathLength_{1024};

godot::CL::TileGraph::TileGraph() : foreign_occupants_({}), vertices_({}) {}

godot::CL::TileGraph::~TileGraph() { destroy(); }

void godot::CL::TileGraph::destroy() {
    const auto size{vertices_.size()};
    for (int32_t i = 0; i < size; i++) {
        TileVertex *tmp{vertices_[i]};
        vertices_[i] = nullptr;
        delete tmp;
    }
    vertices_.clear();
}

godot::PackedVector2Array godot::CL::TileGraph::astar_construct_path(
    Vector2i start, Vector2i end, const TileSurface surface) {
    TileVertex *start_vertex = get_vertex(start);
    TileVertex *end_vertex = get_vertex(end);
    ERR_FAIL_NULL_V_EDMSG(start_vertex, PackedVector2Array(),
                          vformat("start vertex (%d,%d) was not found in graph",
                                  start.x, start.y));
    ERR_FAIL_NULL_V_EDMSG(
        end_vertex, PackedVector2Array(),
        vformat("end vertex (%d,%d) was not found in graph", end.x, end.y));
    PackedVector2Array path{
        astar_construct_path(start_vertex, end_vertex, surface)};
#ifdef CL_TRADING_DEBUG
    String path_str{};
    int64_t size{path.size()};
    for (int i = 0; i < size; i++) {
        Vector2 vec{path[i]};
        path_str += vformat("(%d, %d)", vec.x, vec.y);
        if (i < size - 1) {
            path_str += "->";
        }
    }
    TILELOG(debug_,
            "path constructed, start=(%d, %d), end=(%d, %d), path:\n%s\n",
            start.x, start.y, end.x, end.y, GDSTR(path_str));
#endif
    return path;
}

// https://en.wikipedia.org/wiki/A*_search_algorithm
godot::PackedVector2Array godot::CL::TileGraph::astar_construct_path(
    TileVertex *start, TileVertex *end, const TileSurface surface) {
    // prio queue doesn't contain a method to see if it contains an element
    // so, because im stupid, I have another map that keeps track of that
    AStarPrioQueueMember open_set_members{};
    // keep track of where we came from, so path between
    // start/end can be reconstructed once end is reached
    AStarCameFromMap came_from{};
    // best currently known paths from start to n
    AStarGScoreMap g_score{};
    // best guess for cheapest path from start to end through n
    AStarFScoreMap f_score{};

    // prio queue comparator function, best f_score wins
    auto cmp = [&f_score](TileVertex *a, TileVertex *b) {
        const auto a_score{f_score.find(a)};
        const auto b_score{f_score.find(b)};
        ERR_FAIL_COND_V_EDMSG(
            a_score == f_score.end(), false,
            vformat("a_score (%d,%d) not found", a->coords.x, a->coords.y));
        ERR_FAIL_COND_V_EDMSG(
            b_score == f_score.end(), false,
            vformat("b_score (%d,%d) not found", b->coords.x, b->coords.y));
        return b_score->second.val < a_score->second.val;
    };
    // set of nodes whose edges may need to be visited
    std::priority_queue<TileVertex *, std::vector<TileVertex *>, decltype(cmp)>
        open_set(cmp);

    // setup initial state
    g_score[start] = 0;
    f_score[start] = astar_calculate_heuristic_(start, end);
    open_set.push(start);
    open_set_members[start] = 1;

    while (!open_set.empty()) {
        auto *current = open_set.top();
        open_set.pop();
        open_set_members[current] = 0;
        if (current == end) {
            return astar_reconstruct_path_(start->coords, came_from, current);
        }
        for (auto *edge : current->edges) {
            if (edge->surface != TILE_SURFACE_BRIDGE &&
                (edge->surface != surface || has_occupant(edge->coords))) {
                continue;
            }
            // distance from start to edge through current at cost weight
            auto tmp_g_score{g_score[current].val + edge->weight};
            // if the overall cost is lower
            if (tmp_g_score < g_score[edge].val) {
                // save the node and scores
                came_from[edge] = current;
                g_score[edge] = tmp_g_score;
                f_score[edge] =
                    tmp_g_score + astar_calculate_heuristic_(edge, end);
                // and add to open_set if not already found
                // so its edges can be visited later on
                if (open_set_members[edge] == 0) {
                    open_set.push(edge);
                    open_set_members[edge] = 1;
                }
            }
        }
    }
    // return empty array if no path found
    return PackedVector2Array();
}

godot::PackedVector2Array godot::CL::TileGraph::astar_reconstruct_path_(
    const Vector2i start, AStarCameFromMap came_from, TileVertex *current) {
    auto path = PackedVector2Array();
    if (came_from.size() < 2) {
        return path;
    }
    path.append(current->coords);
    int iters{0};
    TileVertex *tmp{current};
    while (iters < MaxPathLength_) {
        if (tmp->coords == start) {
            break;
        }
        tmp = came_from[tmp];
        path.append(tmp->coords);
        ++iters;
    }
    path.reverse();
    return path;
}

int godot::CL::TileGraph::astar_calculate_heuristic_(TileVertex *current,
                                                     TileVertex *goal) const {
    auto x_diff{current->coords.x - goal->coords.x};
    auto y_diff{current->coords.y - goal->coords.y};
    auto mag{(x_diff * x_diff) + (y_diff * y_diff)};
    // TODO why n * 10?
    return int(std::sqrt(mag) * 10);
}

void godot::CL::TileGraph::add_edge(TileVertex *v1, TileVertex *v2) {
    v1->edges.push_back(v2);
    v2->edges.push_back(v1);
}
void godot::CL::TileGraph::add_edge(const Vector2i v1, const Vector2i v2) {
    TileVertex *t1{get_vertex(v1)};
    ERR_FAIL_NULL_EDMSG(t1, vformat("vertex (%d,%d) not found", v1.x, v1.y));
    TileVertex *t2{get_vertex(v2)};
    ERR_FAIL_NULL_EDMSG(t2, vformat("vertex (%d,%d) not found", v2.x, v2.y));
    add_edge(t1, t2);
}

godot::CL::TileVertex *godot::CL::TileGraph::add_vertex(
    const Vector2i indicies, const int weight, const TileSurface surface) {
    auto *vertex{new TileVertex()};
    vertex->coords = indicies;
    vertex->weight = weight;
    vertex->surface = surface;
    vertices_.push_back(vertex);
    return vertex;
}

godot::CL::TileVertex *godot::CL::TileGraph::get_vertex(
    const Vector2i indicies) const {
    for (auto *vertex : vertices_) {
        if (vertex->coords == indicies) {
            return vertex;
        }
    }
    return nullptr;
}

void godot::CL::TileGraph::add_foreign_occupant(const Vector2i v,
                                                const int kind) {
    auto *vertex = get_vertex(v);
    if (vertex == nullptr) {
        return;
    }
    foreign_occupants_[v] = ForeignOccupant{vertex->surface, kind};
    vertex->surface = TILE_SURFACE_OBSTACLE;
}

void godot::CL::TileGraph::remove_foreign_occupant(const Vector2i v) {
    auto *vertex = get_vertex(v);
    if (vertex == nullptr) {
        return;
    }
    vertex->surface = static_cast<TileSurface>(foreign_occupants_[v].surface);
    foreign_occupants_[v] = ForeignOccupant{0, 0};
}

void godot::CL::TileGraph::reset_occupants_kind(const int kind) {
    auto iter = foreign_occupants_.begin();
    for (; iter != foreign_occupants_.end();) {
        if (iter->second.surface > 0 && iter->second.kind == kind) {
            iter = foreign_occupants_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void godot::CL::TileGraph::print() const {
#ifdef CL_TRADING_DEBUG
    for (const auto &vertex : vertices_) {
        String edges_str{};
        for (const auto &edge : vertex->edges) {
            edges_str += vformat("\t\t-> Has Edges (%d, %d) with weight %d\n",
                                 edge->coords.x, edge->coords.y, edge->weight);
        }
        TILELOG(debug_,
                "Vertex (%d, %d):\n\t-> Has Surface (%d)\n\t-> Has Weight "
                "(%d)\n%s\n",
                vertex->coords.x, vertex->coords.y, vertex->surface,
                vertex->weight, GDSTR(edges_str));
    }
#endif
}
