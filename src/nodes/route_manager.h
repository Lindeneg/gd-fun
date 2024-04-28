#ifndef CL_TRADING_ROUTE_MANAGER_H_
#define CL_TRADING_ROUTE_MANAGER_H_

#include <godot_cpp/classes/node.hpp>

#include "./route.h"

namespace godot::CL {
class CityManager;

class RouteManager : public Node {
    GDCLASS(RouteManager, Node)

   private:
    Dictionary routes_;
    CityManager *city_manager_;

    void handle_dest_reached_(StringName player_name, StringName route_name,
                              VehicleMoveDir direction);
    void handle_timeout_(StringName player_name, StringName route_name);

   protected:
    static void _bind_methods();

   public:
    RouteManager();
    ~RouteManager();

    void _enter_tree() override;

    TypedArray<Route> get_player_routes(const StringName player_name) const;
    Route *get_player_route(const StringName player_name,
                            const StringName route_name) const;
    void add_route(Route *route);
    void remove_route_from_name(const StringName name);
    void remove_route(const Route *route);
};
}  // namespace godot::CL

#endif  // CL_TRADING_ROUTE_MANAGER_H_
