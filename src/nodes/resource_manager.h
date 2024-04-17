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
    Callable resource_clicked_cb_;

    void handle_resource_clicked_(StringName resource_name);

   protected:
    static void _bind_methods();

    void iterate_children_(TypedArray<Entryable> nodes) override;

   public:
    ResourceManager();
    ~ResourceManager();

    const static char *SResourceClicked;

    void lock_all_buttons();
    void unlock_all_buttons();
    void lock_buttons_except(TypedArray<StringName> except);
    void unlock_buttons_except(TypedArray<StringName> except);
    ResourceTile *get_resource(StringName name) const;
    Array get_resources_within_distance(Entryable *from, int distance);
};
}  // namespace godot::CL

#endif  // CL_TRADING_RESOURCE_MANAGER_H_
