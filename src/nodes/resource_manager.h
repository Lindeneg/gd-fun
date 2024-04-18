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

class ResourceManager : public TilePlaceable {
    GDCLASS(ResourceManager, TilePlaceable)

   private:
    Callable resource_clicked_cb_;

    void handle_resource_clicked_(StringName resource_name);

   protected:
    static void _bind_methods();

    void iterate_children_(TypedArray<Entryable> nodes) override;

   public:
    ResourceManager();
    ~ResourceManager();

    const static char *SResourceClicked;

    ResourceTile *get_resource(StringName name) const;
    inline Dictionary get_resources() const { return entries_; }
    inline Array get_resources_within_distance(Entryable *from, int distance) {
        return get_within_distance_(from, distance);
    }
};
}  // namespace godot::CL

#endif  // CL_TRADING_RESOURCE_MANAGER_H_
