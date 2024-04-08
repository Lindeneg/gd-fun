#ifndef CL_TRADING_PLACEABLE_H_
#define CL_TRADING_PLACEABLE_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>

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
    void handle_sprite_tile_manager_notification_(Sprite2D *sprite,
                                                  Node2D *parent);

    virtual void iterate_children_(TypedArray<Node> nodes, Node2D *parent){};

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
