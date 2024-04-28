#ifndef CL_TRADING_ROUTE_H_
#define CL_TRADING_ROUTE_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include "../core/entryable.h"
#include "../core/utils.h"
#include "./city_resource.h"
#include "./player.h"
#include "./trading_vehicle.h"

namespace godot::CL {

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
    bool initial_start_;
    EntryableKind kind_;
    Player *player_;
    // packed array of current route
    TypedArray<Vector2> current_route_;
    Dictionary cargo_;
    // intended tile surface
    TileSurface type_;
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
    void handle_destination_reached_(const int direction);
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
    void _exit_tree() override;

    void start();
    void stop();
    void destroy();

    TypedArray<CityResource> get_current_cargo() const;
    void add_to_current_cargo(const ResourceKind kind, const int amount);
    void remove_from_current_cargo(const ResourceKind kind, const int amount);

    inline Dictionary get_cargo() const { return cargo_; }
    inline void set_cargo(const Dictionary c) { cargo_ = c; }

    inline void set_timeout_duration(const int t) {
        cooldown_timer_->set_wait_time(t);
    }
    inline void start_timer() { cooldown_timer_->start(); }
    inline void stop_timer() { cooldown_timer_->stop(); }

    inline Player *get_player() const { return player_; }
    inline int get_distance() const { return distance_; }
    inline int get_gold_cost() const { return gold_cost_; }
    inline TypedArray<Vector2> get_current_route() const {
        return current_route_;
    }
    inline StringName get_target_entryable() const {
        if (vehicle_->get_move_dir() == VEHICLE_MOVE_DIR_AB) {
            return end_;
        }
        return start_;
    }
    inline RouteState get_route_state() const { return state_; }
    inline TradingVehicle *get_vehicle() const { return vehicle_; }
    inline StringName get_start() const { return start_; }
    inline StringName get_end() const { return end_; }
    inline TileSurface get_type() const { return type_; }
    inline EntryableKind get_kind() const { return kind_; }

    inline void set_current_route(const TypedArray<Vector2> path) {
        current_route_ = path;
    }
    inline void set_player(Player *p) { player_ = p; }
    inline void set_start(const StringName name) { start_ = name; }
    inline void set_end(const StringName name) { end_ = name; }
    inline void set_type(const TileSurface t) { type_ = t; }
    inline void set_kind(const int k) { kind_ = static_cast<EntryableKind>(k); }

    void set_vehicle(TradingVehicle *vehicle);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::RouteState);

#endif  // CL_TRADING_ROUTE_H_
