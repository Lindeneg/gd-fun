#ifndef CL_TRADING_PLACEABLE_H_
#define CL_TRADING_PLACEABLE_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <map>

#include "./entryable.h"

namespace godot::CL {
class TileManager;

enum TilePlaceableKind { PLACEABLE_CITY, PLACEABLE_RESOURCE };

class TilePlaceable : public Node {
    GDCLASS(TilePlaceable, Node)
   private:
    TilePlaceableKind kind_;

   protected:
    static void _bind_methods();

    TileManager *tile_manager_;
    Callable tile_manager_ready_cb_;

    void setup_tile_manager_();

    template <typename TNode>
    inline Array get_within_distance_(std::map<StringName, TNode *> entries,
                                      Entryable *from, int distance) {
        auto onshore_from{from->get_entry_tile(TILE_ENTRY_ONSHORE)};
        auto offshore_from{from->get_entry_tile(TILE_ENTRY_OFFSHORE)};
        Array result{};
        for (auto el : entries) {
            if (el.first == from->get_name()) {
                continue;
            }
            auto to{el.second};
            Dictionary entry_result{};
            entry_result["name"] = el.second->get_name();
            auto onshores{find_entry_path_(distance, onshore_from, to,
                                           TILE_ENTRY_ONSHORE)};
            auto offshores{find_entry_path_(distance, offshore_from, to,
                                            TILE_ENTRY_OFFSHORE)};
            if (onshores.size() > 0 || offshores.size() > 0) {
                entry_result["onshores"] = onshores;
                entry_result["offshores"] = offshores;
                result.push_back(entry_result);
            }
        }
        return result;
    }

    void handle_sprite_tile_manager_notification_(Sprite2D *sprite,
                                                  Node2D *parent);
    void handle_entryable_node_(Entryable *root, Node *node, Node2D *parent);
    TypedArray<Vector2> get_local_path_(PackedVector2Array path);
    Array find_entry_path_(const int max_distance, const Dictionary from,
                           const Entryable *to_entry,
                           const TileEntryType entry_type);

    virtual void iterate_children_(TypedArray<Entryable> nodes){};

   public:
    TilePlaceable();
    TilePlaceable(TilePlaceableKind kind);
    ~TilePlaceable();

    void _enter_tree() override;
    void _exit_tree() override;

    void notify_tile_manager();
};
}  // namespace godot::CL

#endif  // CL_TRADING_PLACEABLE_H_
