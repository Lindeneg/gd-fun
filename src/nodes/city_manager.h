#ifndef CL_TRADING_CITY_MANAGER_H_
#define CL_TRADING_CITY_MANAGER_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include "../core/tile_placeable.h"
#include "./city.h"

namespace godot {
class Sprite2D;
class Node2D;
}  // namespace godot

namespace godot::CL {

class CityManager : public TilePlaceable {
    GDCLASS(CityManager, TilePlaceable)

   private:
    Callable city_clicked_cb_;

    void handle_city_clicked_(StringName city_name);

   protected:
    static void _bind_methods();

    void iterate_children_(TypedArray<Entryable> nodes) override;

   public:
    CityManager();
    ~CityManager();

    const static char *SCityClicked;

    City *get_city(StringName name) const;
    inline Dictionary get_cities() const { return entries_; }
    inline Array get_cities_within_distance(City *from, int distance) {
        return get_within_distance_(from, distance);
    }
};
}  // namespace godot::CL

#endif  // CL_TRADING_CITY_MANAGER_H_
