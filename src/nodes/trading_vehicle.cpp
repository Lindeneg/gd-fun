#include "trading_vehicle.h"

#include <cstdint>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <iostream>

#include "../core/utils.h"

const int32_t godot::CL::TradingVehicle::AnimationSize{4};
const std::string godot::CL::TradingVehicle::AnimationNames
    [godot::CL::TradingVehicle::AnimationSize]{"left", "up", "right", "down"};

godot::CL::TradingVehicle::TradingVehicle()
    : debug_mode_(false),
      speed_(0.0),
      destination_threshold_(0.0),
      state_(VEHICLE_IDLE),
      navigation_target_(Vector2()),
      direction_(Vector2()),
      animated_sprite_(nullptr),
      collision_shape_(nullptr) {}

godot::CL::TradingVehicle::~TradingVehicle() {
    if (animated_sprite_ != nullptr && animated_sprite_->get_owner() == this) {
        std::cout << "TradingVehicle: Deleting animated sprite\n";
        delete animated_sprite_;
    }
    if (collision_shape_ != nullptr && collision_shape_->get_owner() == this) {
        std::cout << "TradingVehicle: Deleting collision shape\n";
        delete collision_shape_;
    }
    animated_sprite_ = nullptr;
    collision_shape_ = nullptr;
}

// this is called on _ready notification at runtime
void godot::CL::TradingVehicle::r_assign_required_components_() {
    if (animated_sprite_ == nullptr) {
        Node* anim{find_child("VehicleAnimation")};
        ERR_FAIL_COND_MSG(anim == nullptr,
                          "required component VehicleAnimation missing");
        animated_sprite_ = static_cast<AnimatedSprite2D*>(anim);
    }
    if (collision_shape_ == nullptr) {
        Node* col{find_child("VehicleCollider")};
        ERR_FAIL_COND_MSG(col == nullptr,
                          "required component VehicleCollider missing");
        collision_shape_ = static_cast<CollisionShape2D*>(col);
    }
}

// this is called on _ready notification in editor
void godot::CL::TradingVehicle::e_assign_required_components_() {
    ERR_FAIL_COND_MSG(animated_sprite_ != nullptr,
                      "component VehicleAnimation already assigned");
    ERR_FAIL_COND_MSG(collision_shape_ != nullptr,
                      "component VehicleCollider already assigned");
    Node* anim{find_child("VehicleAnimation")};
    if (anim == nullptr) {
        animated_sprite_ =
            create_component_<AnimatedSprite2D>("VehicleAnimation");
        initialize_sprite_frames_();
    } else {
        animated_sprite_ = static_cast<AnimatedSprite2D*>(anim);
    }
    Node* col{find_child("VehicleCollider")};
    if (col == nullptr) {
        collision_shape_ =
            create_component_<CollisionShape2D>("VehicleCollider");
        collision_shape_->set_shape(new RectangleShape2D());
    } else {
        collision_shape_ = static_cast<CollisionShape2D*>(col);
    }
}

void godot::CL::TradingVehicle::initialize_sprite_frames_() {
    auto* sprite_frames{new SpriteFrames()};
    if (sprite_frames->has_animation("default")) {
        sprite_frames->remove_animation("default");
    }
    for (const auto animation : AnimationNames) {
        sprite_frames->add_animation(animation.c_str());
    }
    animated_sprite_->set_sprite_frames(sprite_frames);
}

void godot::CL::TradingVehicle::handle_movement(double delta) {
    if (::CL::is_in_editor()) {
        return;
    }
    if (Input::get_singleton()->is_action_just_pressed("mouse_click")) {
        navigation_target_ = get_global_mouse_position();
        state_ = VEHICLE_MOVING;
    }

    if (state_ != VEHICLE_MOVING) {
        return;
    }

    const Vector2 pos{get_position()};
    const Vector2 diff{navigation_target_ - pos};
    if (diff.length() <= destination_threshold_) {
        state_ = VEHICLE_IDLE;  // destination reached
        animated_sprite_->stop();
        return;
    }
    direction_ = diff.normalized();
    set_position(pos + (direction_ * speed_ * delta));
    update_animation();
}

void godot::CL::TradingVehicle::update_animation() {
    const auto anim_idx{static_cast<int32_t>(
        (AnimationSize *
         (direction_.rotated(Math_PI / AnimationSize).angle() + Math_PI) /
         Math_TAU))};
    ERR_FAIL_COND_MSG(anim_idx >= AnimationSize, "anim_idx is out of bounds");
    animated_sprite_->play(AnimationNames[anim_idx].c_str());
}

void godot::CL::TradingVehicle::_ready() {
    if (::CL::is_in_editor()) {
        e_assign_required_components_();
    } else {
        r_assign_required_components_();
    }
}

void godot::CL::TradingVehicle::_process(double delta) {
    if (::CL::is_in_game()) {
        handle_movement(delta);
    }
    emit_debug_signal_();
}

void godot::CL::TradingVehicle::emit_debug_signal_() {
    if (debug_mode_) {
        if (is_moving()) {
            emit_signal("draw_debug_lines", get_position(), navigation_target_);
        }
    } else {
        emit_signal("clear_debug_lines");
    }
}

void godot::CL::TradingVehicle::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_state"), &TradingVehicle::get_state);

    ClassDB::bind_method(D_METHOD("get_navigation_target"),
                         &TradingVehicle::get_navigation_target);
    ClassDB::bind_method(D_METHOD("set_navigation_target", "t"),
                         &TradingVehicle::set_navigation_target);

    ClassDB::bind_method(D_METHOD("handle_movement", "delta"),
                         &TradingVehicle::handle_movement);

    ClassDB::bind_method(D_METHOD("is_moving"), &TradingVehicle::is_moving);

    // NAVIGATION EDITOR PROPS
    ClassDB::bind_method(D_METHOD("get_speed"), &TradingVehicle::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "s"),
                         &TradingVehicle::set_speed);

    ClassDB::bind_method(D_METHOD("get_destination_threshold"),
                         &TradingVehicle::get_destination_threshold);
    ClassDB::bind_method(D_METHOD("set_destination_threshold", "t"),
                         &TradingVehicle::set_destination_threshold);

    ClassDB::add_property_group("TradingVehicle", "Navigation", "");
    ClassDB::add_property(
        "TradingVehicle",
        PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0.0,1000"),
        "set_speed", "get_speed");
    ClassDB::add_property("TradingVehicle",
                          PropertyInfo(Variant::FLOAT, "destination_threshold",
                                       PROPERTY_HINT_RANGE, "0.001,10"),
                          "set_destination_threshold",
                          "get_destination_threshold");

    // DEBUG EDITOR PROPS
    ClassDB::bind_method(D_METHOD("get_debug_mode"),
                         &TradingVehicle::get_debug_mode);
    ClassDB::bind_method(D_METHOD("set_debug_mode", "m"),
                         &TradingVehicle::set_debug_mode);

    ClassDB::add_property_group("TradingVehicle", "Debug", "");
    ClassDB::add_property("TradingVehicle",
                          PropertyInfo(Variant::BOOL, "debug_mode"),
                          "set_debug_mode", "get_debug_mode");

    // SIGNALS
    ClassDB::add_signal("TradingVehicle",
                        MethodInfo("draw_debug_lines",
                                   PropertyInfo(Variant::VECTOR2, "position"),
                                   PropertyInfo(Variant::VECTOR2, "target")));
    ClassDB::add_signal("TradingVehicle", MethodInfo("clear_debug_lines"));
    // BIND ENUMS
    BIND_ENUM_CONSTANT(VEHICLE_IDLE);
    BIND_ENUM_CONSTANT(VEHICLE_MOVING);
    BIND_ENUM_CONSTANT(VEHICLE_ONLOADING);
    BIND_ENUM_CONSTANT(VEHICLE_OFFLOADING);
}
