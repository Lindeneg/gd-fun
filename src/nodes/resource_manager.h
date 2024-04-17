#ifndef CL_TRADING_RESOURCE_MANAGER_H_
#define CL_TRADING_RESOURCE_MANAGER_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>

#include "../core/tile_placeable.h"
#include "./resource.h"

namespace godot {
class Node2D;
}  // namespace godot

namespace godot::CL {

class ResourceManager : public TilePlaceable<ResourceTile> {
    GDCLASS(ResourceManager, TilePlaceable<ResourceTile>)

   protected:
    static void _bind_methods();

    // void handle_entry_clicked_(StringName entry_name) override;

   public:
    ResourceManager();
    ~ResourceManager();

    void _enter_tree() override;
    void _exit_tree() override;
};
}  // namespace godot::CL

#endif  // CL_TRADING_RESOURCE_MANAGER_H_
