#ifndef CL_TRADING_TRADING_VEHICLE_H_
#define CL_TRADING_TRADING_VEHICLE_H_

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/memory.hpp>

namespace godot {
class AnimatedSprite2D;
class CollisionShape2D;
}  // namespace godot

namespace godot::CL {

struct NextNavResult {
    Vector2 target;
    bool is_valid_target;
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
 * TradingVehicles are managed by associated Route.
 *
 * TODO (1)
 * TradingVehicle has build-time relative to
 * the its tier managed by BuildManager.
 * */
class TradingVehicle : public Area2D {
    GDCLASS(TradingVehicle, Area2D)
   private:
    static const char* AnimationNames[];
    static const int32_t AnimationSize;
    // TODO (1) int cargo_space_;
    // TODO (1) int maintenance_cost_;
    // TODO (1) Node* cargo_container_;

    // increment or decrement route_idx
    // i.e are we going from
    // cityX->cityY or cityY->cityX
    bool map_route_inc_;
    int64_t current_map_route_idx_;
    TypedArray<Vector2> map_route_;

    // tier of vehicle
    VehicleTier tier_;
    // speed multiplier
    double speed_;
    // stops movement if distance
    // to nav target is within threshold
    double destination_threshold_;
    // current state
    VehicleState state_;
    // control sprite animations
    AnimatedSprite2D* animated_sprite_;
    // definition of sprite collision shape
    CollisionShape2D* collision_shape_;
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
    // create component and add to tree
    template <typename T>
    T* create_component_() {
        T* obj = memnew(T);
        add_child(obj);
        obj->set_owner(this);
        return obj;
    }

   protected:
    static void _bind_methods();

   public:
    TradingVehicle();
    ~TradingVehicle();

    const static char* SDestReached;

    void _ready() override;
    void _process(double delta) override;

    void start_navigating();
    void stop_navigating();

    inline void clear_map_path() { map_route_.clear(); }
    inline bool is_moving() const { return state_ == VEHICLE_MOVING; }

    inline double get_destination_threshold() const {
        return destination_threshold_;
    }
    inline double get_speed() const { return speed_; }
    inline VehicleState get_state() const { return state_; }
    inline VehicleTier get_tier() const { return tier_; }
    inline Vector2 get_navigation_target() const { return navigation_target_; }

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
    inline void set_tier(const VehicleTier t) { tier_ = t; }
    inline void set_speed(const double s) { speed_ = s; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::VehicleState);
VARIANT_ENUM_CAST(godot::CL::VehicleTier);

#endif  // CL_TRADING_TRADING_VEHICLE_H_
