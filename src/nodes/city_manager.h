#ifndef CL_TRADING_CITY_MANAGER_H_
#define CL_TRADING_CITY_MANAGER_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>

#include "../core/tile_placeable.h"
#include "./city.h"

namespace godot {
class Sprite2D;
class Node2D;
}  // namespace godot

namespace godot::CL {
class CityManager : public TilePlaceable<City> {
    GDCLASS(CityManager, TilePlaceable<City>)

   protected:
    Callable entity_clicked_cb_;

    static void _bind_methods();

    void handle_city_clicked_(StringName entry_name);
    void iterate_children_(TypedArray<Entryable> nodes) override;

   public:
    CityManager();
    ~CityManager();

    void _enter_tree() override;
    void _exit_tree() override;

    Array get_cities_within_distance(City *from, int distance);
};
}  // namespace godot::CL

#endif  // CL_TRADING_CITY_MANAGER_H_
