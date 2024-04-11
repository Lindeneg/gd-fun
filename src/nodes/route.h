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
class ResourceManager;
class TradingVehicle;

enum RouteState { ROUTE_INACTIVE, ROUTE_ACTIVE };
enum RouteKind { ROUTE_CITY_CITY, ROUTE_CITY_RESOURCE };

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
    bool initial_start_;
    RouteKind kind_;
    // packed array of current route
    PackedVector2Array current_route_;
    // intended tile surface
    TileSurface type_;
    // pointers to needed managers
    TileManager *tile_manager_;
    CityManager *city_manager_;
    ResourceManager *resource_manager_;
    // signal Callables
    Callable timeout_cb_;
    Callable dest_reached_cb_;
    // from and to entries
    StringName start_;
    StringName end_;
    // distance betweem them
    int distance_;
    // current state
    RouteState state_;
    Timer *cooldown_timer_;
    // cost to player
    int gold_cost_;
    // trading vehicle instance
    TradingVehicle *vehicle_;

    // callback to handle reaction
    // to Timer.timeout signal
    void handle_timeout_();
    // callback to handle vehicle
    // destination reached signal
    void handle_destination_reached_(const Vector2 dest);
    // converts from tile coords to local coords
    TypedArray<Vector2> get_local_path_();
    // finds an entry respecting surface
    Vector2 get_entry_tile_(StringName name) const;
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

    bool start();
    void stop();
    void destroy();
    void change_trading_vehicle();
    void change_route_plan();

    inline int get_distance() const { return distance_; }
    inline int get_gold_cost() const { return gold_cost_; }
    inline PackedVector2Array get_current_route() const {
        return current_route_;
    }
    inline RouteState get_route_state() const { return state_; }
    inline TradingVehicle *get_vehicle() const { return vehicle_; }
    inline StringName get_start() const { return start_; }
    inline StringName get_end() const { return end_; }
    inline TileSurface get_type() const { return type_; }
    inline RouteKind get_kind() const { return kind_; }

    inline void set_start(const StringName name) { start_ = name; }
    inline void set_end(const StringName name) { end_ = name; }
    inline void set_type(const TileSurface t) { type_ = t; }
    inline void set_kind(const int k) { kind_ = static_cast<RouteKind>(k); }

    void set_vehicle(TradingVehicle *vehicle);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::RouteState);
VARIANT_ENUM_CAST(godot::CL::RouteKind);

#endif  // CL_TRADING_ROUTE_H_
