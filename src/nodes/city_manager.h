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

    Callable city_clicked_cb_;

    void handle_city_clicked_(StringName city_name);
    Array find_entry_path_(const int max_distance, const Dictionary from,
                           const Entryable *to_entry,
                           const TileEntryType entry_type);

   protected:
    static void _bind_methods();

    void iterate_children_(TypedArray<Node> nodes, Node2D *parent) override;

   public:
    CityManager();
    ~CityManager();

    const static char *SCityClicked;

    void lock_all_buttons();
    void unlock_all_buttons();
    void lock_buttons_except(TypedArray<StringName> except);
    void unlock_buttons_except(TypedArray<StringName> except);
    City *get_city(StringName name) const;
    Array get_cities_within_distance(City *from, int distance);
};
}  // namespace godot::CL

#endif  // CL_TRADING_CITY_MANAGER_H_
