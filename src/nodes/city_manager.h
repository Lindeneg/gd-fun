#ifndef CL_TRADING_CITY_MANAGER_H_
#define CL_TRADING_CITY_MANAGER_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <map>

#include "../core/tile_placeable.h"
#include "./city.h"

namespace godot {
class Sprite2D;
class Node2D;
}  // namespace godot

namespace godot::CL {
class TileManager;

class CityManager : public TilePlaceable {
    GDCLASS(CityManager, TilePlaceable)

   private:
    std::map<StringName, City *> cities_;

   protected:
    static void _bind_methods();

    void iterate_children_(TypedArray<Node> nodes, Node2D *parent) override;

   public:
    CityManager();
    ~CityManager();

    City *get_city(StringName name) const;
};
}  // namespace godot::CL

#endif  // CL_TRADING_CITY_MANAGER_H_
