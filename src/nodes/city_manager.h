#ifndef CL_TRADING_CITY_MANAGER_H_
#define CL_TRADING_CITY_MANAGER_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <map>

#include "./city.h"

namespace godot {
class Sprite2D;
class Node2D;
}  // namespace godot

namespace godot::CL {
class TileManager;

class CityManager : public Node {
    GDCLASS(CityManager, Node)

   private:
    std::map<StringName, City*> cities_;
    TileManager* tile_manager_;
    Callable tile_manager_ready_cb_;

    void setup_tile_manager_();
    void handle_sprite_tile_manager_notification_(Sprite2D* sprite,
                                                  Node2D* parent);
    void iterate_children_(TypedArray<Node> nodes, Node2D* parent);

   protected:
    static void _bind_methods();

   public:
    CityManager();
    ~CityManager();

    void _enter_tree() override;
    void _exit_tree() override;

    City* get_city(StringName name) const {
        auto city = cities_.find(name);
        if (city == cities_.end()) {
            return nullptr;
        }
        return city->second;
    }
    void notify_tile_manager_of_cities();
};
}  // namespace godot::CL

#endif  // CL_TRADING_CITY_MANAGER_H_
