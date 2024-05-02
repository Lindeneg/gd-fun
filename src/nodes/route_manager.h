#ifndef CL_TRADING_ROUTE_MANAGER_H_
#define CL_TRADING_ROUTE_MANAGER_H_

#include <godot_cpp/classes/node.hpp>

#include "./route.h"

namespace godot::CL {
class CityManager;
class ResourceManager;

class RouteManager : public Node {
    GDCLASS(RouteManager, Node)

   private:
    bool debug_;
    Dictionary routes_;
    CityManager *city_manager_;
    ResourceManager *resource_manager_;
    Callable offload_cargo_cb_;
    Callable onload_cargo_cb_;
    Callable offload_finished_cb_;
    Callable onload_finished_cb_;

    void handle_offload_cargo_(StringName player_name, StringName route_name,
                               ResourceKind kind);
    void handle_onload_cargo_(StringName player_name, StringName route_name,
                              ResourceKind kind);
    void handle_offload_finished_(StringName player_name,
                                  StringName route_name);
    void handle_onload_finished_(StringName player_name, StringName route_name);

   protected:
    static void _bind_methods();

   public:
    RouteManager();
    ~RouteManager();

    DEBUG_METHODS()

    void _enter_tree() override;

    TypedArray<Route> get_player_routes(const StringName player_name) const;
    Route *get_player_route(const StringName player_name,
                            const StringName route_name) const;
    void add_route(Route *route);
    void remove_route_from_name(const StringName player_name,
                      const StringName route_name);
    void remove_route(Route *route);
};
}  // namespace godot::CL

#endif  // CL_TRADING_ROUTE_MANAGER_H_
