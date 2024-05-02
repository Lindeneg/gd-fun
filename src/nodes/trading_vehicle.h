#ifndef CL_TRADING_TRADING_VEHICLE_H_
#define CL_TRADING_TRADING_VEHICLE_H_

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/templates/vector.hpp>

#include "../core/utils.h"

namespace godot {
class AnimatedSprite2D;
class CollisionShape2D;
class PackedScene;
}  // namespace godot

namespace godot::CL {

struct NextNavResult {
    Vector2 target;
    bool is_valid_target;
};

enum VehicleMoveDir {
    VEHICLE_MOVE_DIR_AB,
    VEHICLE_MOVE_DIR_BA,
};

// vehicle tiers determine
// efficiency and build/gost cost
enum VehicleTier {
    VEHICLE_TIER_BUDGET,
    VEHICLE_TIER_COMMON,
    VEHICLE_TIER_PREMIUM
};

// possible vehicle states
enum VehicleState {
    VEHICLE_IDLE,
    VEHICLE_MOVING,
    VEHICLE_ONLOADING,
    VEHICLE_OFFLOADING
};

/* TradingVehicle are part of Route and
 * is anything that can move, has animations
 * and carries cargo from A<->B.
 *
 * TradingVehicles are managed by associated Route.*/
class TradingVehicle : public Area2D {
    GDCLASS(TradingVehicle, Area2D)
   private:
    static const char *AnimationNames[];
    static const int32_t AnimationSize;

    bool debug_;
    // are we going from
    // cityX->cityY or cityY->cityX
    VehicleMoveDir move_dir_;
    int64_t current_map_route_idx_;
    TypedArray<Vector2> map_route_;
    TileSurface tile_surface_;

    // tier of vehicle
    VehicleTier tier_;
    // speed multiplier
    double speed_;
    int cost_;
    int upkeep_;
    int cargo_space_;
    // stops movement if distance
    // to nav target is within threshold
    double destination_threshold_;
    // current state
    VehicleState state_;
    // control sprite animations
    AnimatedSprite2D *animated_sprite_;
    // definition of sprite collision shape
    CollisionShape2D *collision_shape_;
    // movement direction
    Vector2 direction_;
    // moves towards target if VehicleState is moving
    Vector2 navigation_target_;

    // get next navigation target
    NextNavResult get_next_navigation_target_();
    // handle movement towards current navigation target
    void handle_movement_(double delta);
    // update animation given current direction
    void update_animation_(const bool stop = false);
    // make sure required components are
    // created/assigned when in editor
    void e_assign_required_components_();
    // make sure required components are assigned at runtime
    // asserts required components are in tree
    void r_assign_required_components_();
    // create sprite frames and set required animation names
    void initialize_sprite_frames_();

   protected:
    static void _bind_methods();

   public:
    TradingVehicle();
    ~TradingVehicle();

    const static char *SDestReached;

    void _ready() override;
    void _process(double delta) override;

    void start_navigating();
    void stop_navigating();
    void switch_move_dir();

    DEBUG_METHODS()

    inline void clear_map_path() { map_route_.clear(); }
    inline bool is_moving() const { return state_ == VEHICLE_MOVING; }

    inline double get_destination_threshold() const {
        return destination_threshold_;
    }
    inline double get_speed() const { return speed_; }
    inline int get_cost() const { return cost_; }
    inline int get_upkeep() const { return upkeep_; }
    inline int get_cargo_space() const { return cargo_space_; }
    inline VehicleState get_state() const { return state_; }
    inline VehicleMoveDir get_move_dir() const { return move_dir_; }
    inline VehicleTier get_tier() const { return tier_; }
    inline Vector2 get_navigation_target() const { return navigation_target_; }
    inline TypedArray<Vector2> get_map_path() const { return map_route_; }
    inline TileSurface get_tile_surface() const { return tile_surface_; }

    inline void set_map_path(const TypedArray<Vector2> v) {
        map_route_ = v;
        current_map_route_idx_ = 0;
    }
    inline void set_navigation_target(const Vector2 t) {
        navigation_target_ = t;
    }
    inline void set_destination_threshold(const double t) {
        destination_threshold_ = t;
    }
    inline void set_state(const VehicleState s) { state_ = s; }
    inline void set_tier(const VehicleTier t) { tier_ = t; }
    inline void set_speed(const double s) { speed_ = s; }
    inline void set_cost(const int c) { cost_ = c; }
    inline void set_upkeep(const int u) { upkeep_ = u; }
    inline void set_cargo_space(const int c) { cargo_space_ = c; }
    inline void set_tile_surface(const TileSurface s) { tile_surface_ = s; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::VehicleState);
VARIANT_ENUM_CAST(godot::CL::VehicleTier);
VARIANT_ENUM_CAST(godot::CL::VehicleMoveDir);

#endif  // CL_TRADING_TRADING_VEHICLE_H_
