#ifndef CL_TRADING_TRADING_VEHICLE_H_
#define CL_TRADING_TRADING_VEHICLE_H_

#include <cstddef>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <string>

namespace godot::CL {

enum VehicleState {
    VEHICLE_IDLE = 0,
    VEHICLE_MOVING = 1,
    VEHICLE_ONLOADING = 2,
    VEHICLE_OFFLOADING = 3
};

/* TradingVehicle is anything that can move,
 * has animations and carries cargo from A<->B. */
class TradingVehicle : public Area2D {
    GDCLASS(TradingVehicle, Area2D)
   private:
    // draw some helper lines
    bool debug_mode_;
    // TODO int cargo_space_;
    // TODO int maintenance_cost_;
    // TODO Node* cargo_container_;
    // TODO Route route_;

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

    // update animation given current direction
    void update_animation();
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
    T* create_component_(String name) {
        T* obj = new T();
        obj->set_name(name);
        add_child(obj);
        obj->set_owner(this);
        return obj;
    }

   protected:
    static void _bind_methods();

   public:
    // left,up,right,down
    // godot::String is not available when AnimationNames
    // is initialized, so std::string will do
    static const std::string AnimationNames[4];
    static const std::size_t AnimationSize;

    TradingVehicle();
    ~TradingVehicle();

    void _ready() override;

    void handle_movement(double delta);

    inline double get_destination_threshold() const {
        return destination_threshold_;
    }
    inline bool get_debug_mode() const { return debug_mode_; }
    inline double get_speed() const { return speed_; }
    inline VehicleState get_state() const { return state_; }
    inline Vector2 get_navigation_target() const { return navigation_target_; }
    inline void set_debug_mode(const bool m) { debug_mode_ = m; }
    inline void set_speed(const double s) { speed_ = s; }
    inline bool is_moving() const { return state_ == VEHICLE_MOVING; }
    inline void set_navigation_target(const Vector2 t) {
        navigation_target_ = t;
    }
    inline void set_destination_threshold(const double t) {
        destination_threshold_ = t;
    }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::VehicleState);

#endif  // CL_TRADING_TRADING_VEHICLE_H_
