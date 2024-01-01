#include "tile_graph.h"

#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <iostream>
#include <utility>
#include <vector>

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

// TODO
godot::PackedVector2Array godot::CL::TileGraph::construct_path(
    const Vector2i start, const Vector2i end, const TileMat mat) {
    auto array{PackedVector2Array()};
    return array;
}

void godot::CL::TileGraph::add_edge(TileVertex* v1, TileVertex* v2,
                                    const int weight) {
    v1->edges.push_back(std::make_pair(v2, weight));
    v2->edges.push_back(std::make_pair(v1, weight));
}
bool godot::CL::TileGraph::add_edge(const Vector2i v1, const Vector2i v2,
                                    const int weight) {
    TileVertex* t1{get_vertex(v1)};
    if (t1 == nullptr) {
        return false;
    }
    TileVertex* t2{get_vertex(v2)};
    if (t2 == nullptr) {
        return false;
    }
    add_edge(t1, t2, weight);
    return true;
}

godot::CL::TileVertex* godot::CL::TileGraph::add_vertex(const Vector2i indicies,
                                                        const int weight,
                                                        const TileMat mat,
                                                        TileVertex* previous) {
    auto* vertex{new TileVertex()};
    vertex->x = indicies.x;
    vertex->y = indicies.y;
    vertex->weight = weight;
    vertex->mat = mat;
    vertex->previous = previous;
    vertices_.push_back(vertex);
    return vertex;
}

godot::CL::TileVertex* godot::CL::TileGraph::get_vertex(
    const Vector2i indicies) const {
    for (const auto& vertex : vertices_) {
        if (vertex->x == indicies.x && vertex->y == indicies.y) {
            return vertex;
        }
    }
    return nullptr;
}

void godot::CL::TileGraph::print() const {
    for (const auto& vertex : vertices_) {
        std::cout << "Vertex (" << vertex->x << ", " << vertex->y
                  << "):" << '\n';
        std::cout << "  -> Has Material (" << vertex->mat << ")\n";
        for (const auto& edge : vertex->edges) {
            std::cout << "      -> Has Edges (" << edge.first->x << ", "
                      << edge.first->y << ") with weight " << edge.second
                      << '\n';
        }
        std::cout << '\n';
    }
}
