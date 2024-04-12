#ifndef CL_TRADING_UTILS_H_
#define CL_TRADING_UTILS_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#define NAMEOF(name) #name
#define MAKE_RESOURCE_TYPE_HINT(m_type) \
    vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, m_type)

namespace godot {
class String;
class StringName;
class Callable;
}  // namespace godot

namespace godot::CL {

enum TileSurface {
    TILE_SURFACE_NONE,
    TILE_SURFACE_GROUND,
    TILE_SURFACE_WATER,
    TILE_SURFACE_OBSTACLE,
    TILE_SURFACE_BRIDGE
};

enum CollisionLayer {
    COLLISION_LAYER_VEHICLE = 1,
    COLLISION_LAYER_CITY = 2,
    COLLISION_LAYER_RESOURCE = 3
};

class Utils : public Object {
    GDCLASS(Utils, Object)

   protected:
    static void _bind_methods();

   public:
    template <typename T>
    static T *create_component(Node *owner, bool add_child = true) {
        T *obj = memnew(T);
        if (add_child) {
            owner->add_child(obj);
            obj->set_owner(owner);
        }
        return obj;
    }

    static bool is_in_editor();
    static bool is_in_game();
    static const char *convert_gd_string(const Node *n);
    static const char *convert_gd_string(String s);
    static const char *convert_gd_string(StringName s);
    static void connect(Node *node, const StringName name,
                        const Callable &callable);
    static void disconnect(Node *node, const StringName name,
                           const Callable &callable);
    static void queue_delete(Node *node);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::TileSurface);
VARIANT_ENUM_CAST(godot::CL::CollisionLayer);

#endif  // CL_TRADING_UTILS_H_
