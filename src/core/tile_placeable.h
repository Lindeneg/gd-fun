#ifndef CL_TRADING_PLACEABLE_H_
#define CL_TRADING_PLACEABLE_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/dictionary.hpp>

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

    Dictionary entries_;
    TileManager *tile_manager_;
    Callable tile_manager_ready_cb_;

    void setup_tile_manager_();

    Array get_within_distance_(Entryable *from, int distance);
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

    void lock_all_buttons();
    void unlock_all_buttons();
    void lock_buttons_except(TypedArray<StringName> except);
    void unlock_buttons_except(TypedArray<StringName> except);
    void notify_tile_manager();
};
}  // namespace godot::CL

#endif  // CL_TRADING_PLACEABLE_H_
