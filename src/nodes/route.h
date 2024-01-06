#ifndef CL_TRADING_ROUTE_H_
#define CL_TRADING_ROUTE_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot::CL {

class City;
class TradingVehicle;
class Timer;

// possible route states
enum RouteState { ROUTE_INACTIVE, ROUTE_ACTIVE };

/* Route connects two Cities via TradingVehicle to trade ressources.
 *
 * TODO (1)
 * A Route is dynamic and the Player is allowed to change important
 * aspect of it (cities and vehicle) at will against a gold cost.
 *
 * TODO (1)
 * Player can also delete Route and recoup some of its gold cost.
 *
 * TODO (4)
 * All Routes are controlled by RouteManager. */
class Route : public Node {
    GDCLASS(Route, Node)

   private:
    // current state
    RouteState state_;
    Timer* cooldown_timer_;
    // connected city 1
    City const* c1_;
    // connnected city 2
    City const* c2_;
    // distance betweem them
    int distance_;
    // cost to player
    int gold_cost_;
    // route trading vehicle
    // Route is owner of ptr
    TradingVehicle* vehicle_;

   protected:
    static void _bind_methods();

   public:
    Route();
    ~Route();

    void _ready() override;
    // void _process(double delta) override;

    void change_trading_vehicle();
    void change_route_plan();

    inline int get_distance() const { return distance_; }
    inline int get_gold_cost() const { return gold_cost_; }
    inline RouteState get_route_state() const { return state_; }
    inline TradingVehicle* get_vehicle() const { return vehicle_; }
    inline const City* get_city_one() const { return c1_; }
    inline const City* get_city_two() const { return c2_; }

    void set_vehicle(TradingVehicle* vehicle);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::RouteState);

#endif  // CL_TRADING_ROUTE_H_
