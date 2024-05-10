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

/* Route connects two Entryable's via TradingVehicle
 * to on-and-off-load ressources. */
class Route : public Node {
    GDCLASS(Route, Node)

   private:
    bool debug_;
    bool initial_start_;
    EntryableKind kind_;
    Player *player_;
    TypedArray<Vector2> current_route_;
    Dictionary cargo_;
    TileSurface type_;
    Callable timeout_cb_;
    Callable dest_reached_cb_;
    Callable upkeep_required_cb_;
    StringName start_;
    StringName end_;
    int total_profits_;
    int distance_;
    int current_cargo_idx_;
    RouteState state_;
    Timer *cooldown_timer_;
    int gold_cost_;
    TradingVehicle *vehicle_;

    void handle_timeout_();
    void handle_upkeep_required_();
    void handle_destination_reached_(const int direction);
    void setup_timer_from_tree_or_create_();
    void setup_vehicle_from_tree_();

   protected:
    static void _bind_methods();

   public:
    Route();
    ~Route();

    DEBUG_METHODS()

    const static char *SProfitsChanged;
    const static char *SOnloadCargo;
    const static char *SOnloadCargoFinished;
    const static char *SOffloadCargo;
    const static char *SOffloadCargoFinished;

    void _ready() override;
    void _exit_tree() override;

    void start();
    void stop();
    void destroy();

    void switch_dir();
    void receive_current_resource(int amount);
    void consume_current_resource(int amount);
    void go_to_next_cargo();

    void queue_offload_cargo();
    void queue_onload_cargo();

    TypedArray<CityResource> get_current_cargo() const;

    inline int get_total_profits() const { return total_profits_; }
    inline void add_to_total_profits(const int a) {
        total_profits_ += a;
        emit_signal(SProfitsChanged, total_profits_);
    }

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
    inline StringName get_target_entryable_inv() const {
        if (vehicle_->get_move_dir() == VEHICLE_MOVE_DIR_BA) {
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
