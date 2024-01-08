#ifndef CL_TRADING_ROUTE_H_
#define CL_TRADING_ROUTE_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include "../core/utils.h"
#include "./tile_manager.h"

namespace godot::CL {

class CityManager;
class TradingVehicle;

enum RouteState { ROUTE_INACTIVE, ROUTE_ACTIVE };

/* Route connects two Cities via TradingVehicle to trade ressources.
 *
 * TODO (1)
 * A Route is dynamic and the Player is allowed to change important
 * aspect of it (cities and vehicle) at will against a gold cost.
 *
 * TODO (1)
 * Player can also delete Route and recoup some of its gold cost. */
class Route : public Node {
    GDCLASS(Route, Node)

   private:
    // draw route (if any)
    bool debug_mode_;
    // packed array of current route
    PackedVector2Array current_route_;
    // intended tile surface
    TileSurface type_;
    // pointers to needed managers
    TileManager* tile_manager_;
    CityManager* city_manager_;
    // signal Callables
    Callable timeout_cb_;
    Callable dest_reached_cb_;
    // city id 1
    StringName c1_;
    // city id 2
    StringName c2_;
    // current state
    RouteState state_;
    Timer* cooldown_timer_;
    // distance betweem them
    int distance_;
    // cost to player
    int gold_cost_;
    // trading vehicle instance
    TradingVehicle* vehicle_;

    // callback to handle reaction
    // to Timer.timeout signal
    void handle_timeout_();
    // callback to handle vehicle
    // destination reached signal
    void handle_destination_reached_(const Vector2 dest);
    // emits debug signal
    void emit_debug_signal_();
    // converts from tile coords to local coords
    TypedArray<Vector2> get_local_path_();
    // finds an entry to city respecting type_
    Vector2 get_city_entry_(StringName city_name) const;
    // checks if tile and city managers are assinged
    bool has_required_managers_() const;
    // checks if timer already in tree,
    // if so, assigns it to cooldown_timer_
    // else creates a new instance that is
    // owned by the Route itself
    void setup_timer_from_tree_or_create_();
    // checks if vehicle is already in tree
    // and if so, assigns it to vehicle_
    void setup_vehicle_from_tree_();

   protected:
    static void _bind_methods();

   public:
    Route();
    ~Route();

    void _ready() override;
    void _enter_tree() override;
    void _exit_tree() override;

    bool start(const bool initial_start = false);
    void stop();
    void destroy();
    void change_trading_vehicle();
    void change_route_plan();

    inline int get_distance() const { return distance_; }
    inline int get_gold_cost() const { return gold_cost_; }
    inline bool get_debug_mode() const { return debug_mode_; }
    inline RouteState get_route_state() const { return state_; }
    inline TradingVehicle* get_vehicle() const { return vehicle_; }
    inline StringName get_city_one() const { return c1_; }
    inline StringName get_city_two() const { return c2_; }
    inline TileSurface get_type() const { return type_; }

    void set_debug_mode(const bool m);
    inline void set_city_one(const StringName id) { c1_ = id; }
    inline void set_city_two(const StringName id) { c2_ = id; }
    inline void set_type(const TileSurface t) { type_ = t; }

    void set_vehicle(TradingVehicle* vehicle);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::RouteState);

#endif  // CL_TRADING_ROUTE_H_
