#include "tile_graph.h"

#include <cmath>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <iostream>
#include <vector>

const int godot::CL::TileGraph::MaxPathLength_{1024};

godot::CL::TileGraph::TileGraph() : vertices_({}) {}

godot::CL::TileGraph::~TileGraph() {
#ifdef CL_TRADING_DEBUG
    std::cout << "TileGraph: Freeing tile vertices\n";
#endif
    destroy();
}

void godot::CL::TileGraph::destroy() {
    const auto size{vertices_.size()};
    for (int32_t i = 0; i < size; i++) {
        TileVertex* tmp{vertices_[i]};
        vertices_[i] = nullptr;
        delete tmp;
    }
    vertices_.clear();
}

godot::PackedVector2Array godot::CL::TileGraph::astar_construct_path(
    Vector2i start, Vector2i end, const TileMat mat) {
    auto* start_vertex = get_vertex(start);
    auto* end_vertex = get_vertex(end);
    // TODO assert both are non-null ptr
    return astar_construct_path(start_vertex, end_vertex, mat);
}

// first time implementing a star, this seems so extensive in space
godot::PackedVector2Array godot::CL::TileGraph::astar_construct_path(
    TileVertex* start, TileVertex* end, const TileMat mat) {
    AStarPrioQueue open_set{};
    AStarPrioQueueMember open_set_members{};
    AStarCameFromMap came_from{};
    AStarGScoreMap g_score{};
    AStarFScoreMap f_score{};
    g_score[start] = 0;
    f_score[start] = astar_calculate_heuristic_(start, end);
    open_set.push(start);
    open_set_members[start] = 1;
    while (!open_set.empty()) {
        auto* current = open_set.top();
        open_set.pop();
        if (current == end) {
            return astar_reconstruct_path_(start->coords, came_from, current);
        }
        for (auto* edge : current->edges) {
            auto tmp_g_score =
                g_score[current].val + astar_calculate_cost_(current, edge);
            if (tmp_g_score < g_score[edge].val) {
                came_from[edge] = current;
                g_score[edge] = tmp_g_score;
                f_score[edge] =
                    tmp_g_score + astar_calculate_heuristic_(edge, end);
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
    const Vector2i start, AStarCameFromMap came_from, TileVertex* current) {
    auto path = PackedVector2Array();
    if (came_from.size() < 2) {
        return path;
    }
    path.append(current->coords);
    int iters{0};
    TileVertex* tmp{current};
    while (iters < MaxPathLength_) {
        if (tmp->coords == start) {
            break;
        }
        tmp = came_from[tmp];
        path.append(tmp->coords);
    }
    path.reverse();
    return path;
}

int godot::CL::TileGraph::astar_calculate_heuristic_(TileVertex* current,
                                                     TileVertex* goal) const {
    auto x_diff = current->coords.x - goal->coords.x;
    auto y_diff = current->coords.y - goal->coords.y;
    auto mag = (x_diff * x_diff) + (y_diff * y_diff);
    return std::sqrt(mag);
}

int godot::CL::TileGraph::astar_calculate_cost_(TileVertex* current,
                                                TileVertex* neighbor) const {
    return int((current->weight + neighbor->weight) / 2.0);
}

void godot::CL::TileGraph::add_edge(TileVertex* v1, TileVertex* v2) {
    v1->edges.push_back(v2);
    v2->edges.push_back(v1);
}
bool godot::CL::TileGraph::add_edge(const Vector2i v1, const Vector2i v2) {
    TileVertex* t1{get_vertex(v1)};
    if (t1 == nullptr) {
        return false;
    }
    TileVertex* t2{get_vertex(v2)};
    if (t2 == nullptr) {
        return false;
    }
    add_edge(t1, t2);
    return true;
}

godot::CL::TileVertex* godot::CL::TileGraph::add_vertex(const Vector2i indicies,
                                                        const int weight,
                                                        const TileMat mat,
                                                        TileVertex* previous) {
    auto* vertex{new TileVertex()};
    vertex->coords = indicies;
    vertex->weight = weight;
    vertex->mat = mat;
    vertex->previous = previous;
    vertices_.push_back(vertex);
    return vertex;
}

godot::CL::TileVertex* godot::CL::TileGraph::get_vertex(
    const Vector2i indicies) const {
    for (auto* vertex : vertices_) {
        if (vertex->coords == indicies) {
            return vertex;
        }
    }
    return nullptr;
}

void godot::CL::TileGraph::print() const {
    for (const auto& vertex : vertices_) {
        std::cout << "Vertex (" << vertex->coords.x << ", " << vertex->coords.y
                  << "):" << '\n';
        std::cout << "  -> Has Material (" << vertex->mat << ")\n";
        std::cout << "  -> Has Weight (" << vertex->weight << ")\n";
        for (const auto& edge : vertex->edges) {
            std::cout << "      -> Has Edges (" << edge->coords.x << ", "
                      << edge->coords.y << ") with weight " << edge->weight
                      << '\n';
        }
        std::cout << '\n';
    }
}
