#ifndef CL_TRADING_RESOURCE_MANAGER_H_
#define CL_TRADING_RESOURCE_MANAGER_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <map>

#include "../core/tile_placeable.h"
#include "./resource.h"

namespace godot {
class Node2D;
}  // namespace godot

namespace godot::CL {

class ResourceManager : public TilePlaceable {
    GDCLASS(ResourceManager, TilePlaceable)

   private:
    std::map<StringName, CL::ResourceTile *> resources_;

   protected:
    static void _bind_methods();

    //void iterate_children_(TypedArray<Node> nodes, Node2D *parent) override;

   public:
    ResourceManager();
    ~ResourceManager();

    ResourceTile *get_resource(StringName name) const;
};
}  // namespace godot::CL

#endif  // CL_TRADING_RESOURCE_MANAGER_H_
