#include "trading_vehicle.h"

#include <cstdint>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <iostream>

#include "../core/utils.h"

const int32_t godot::CL::TradingVehicle::AnimationSize{4};
const char* godot::CL::TradingVehicle::AnimationNames
    [godot::CL::TradingVehicle::AnimationSize]{"left", "up", "right", "down"};

godot::CL::TradingVehicle::TradingVehicle()
    : debug_mode_(false),
      map_route_inc_(true),
      map_route_(TypedArray<Vector2>()),
      current_map_route_idx_(0),
      speed_(0.0),
      destination_threshold_(0.0),
      state_(VEHICLE_IDLE),
      navigation_target_(Vector2()),
      direction_(Vector2()),
      animated_sprite_(nullptr),
      collision_shape_(nullptr) {}

godot::CL::TradingVehicle::~TradingVehicle() {
    if (animated_sprite_ != nullptr) {
#ifdef CL_TRADING_DEBUG
        std::cout << "TradingVehicle: Freeing animated sprite\n";
#endif
        animated_sprite_->queue_free();
    }
    if (collision_shape_ != nullptr) {
#ifdef CL_TRADING_DEBUG
        std::cout << "TradingVehicle: Freeing collision shape\n";
#endif
        collision_shape_->queue_free();
    }
    animated_sprite_ = nullptr;
    collision_shape_ = nullptr;
}

// this is called on _ready notification at runtime
void godot::CL::TradingVehicle::r_assign_required_components_() {
    if (animated_sprite_ == nullptr) {
        Node* anim{find_child("VehicleAnimation")};
        ERR_FAIL_NULL_MSG(anim, "required component VehicleAnimation missing");
        animated_sprite_ = static_cast<AnimatedSprite2D*>(anim);
    }
    if (collision_shape_ == nullptr) {
        Node* col{find_child("VehicleCollider")};
        ERR_FAIL_NULL_MSG(col, "required component VehicleCollider missing");
        collision_shape_ = static_cast<CollisionShape2D*>(col);
    }
}

// this is called on _ready notification in editor
void godot::CL::TradingVehicle::e_assign_required_components_() {
    ERR_FAIL_COND_EDMSG(animated_sprite_ != nullptr,
                        "component VehicleAnimation already assigned");
    ERR_FAIL_COND_EDMSG(collision_shape_ != nullptr,
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
        collision_shape_->set_shape(memnew(RectangleShape2D));
    } else {
        collision_shape_ = static_cast<CollisionShape2D*>(col);
    }
}

void godot::CL::TradingVehicle::initialize_sprite_frames_() {
    auto* sprite_frames{memnew(SpriteFrames)};
    if (sprite_frames->has_animation("default")) {
        sprite_frames->remove_animation("default");
    }
    for (const auto animation : AnimationNames) {
        sprite_frames->add_animation(animation);
    }
    animated_sprite_->set_sprite_frames(sprite_frames);
}

void godot::CL::TradingVehicle::handle_movement_(double delta) {
    if (state_ != VEHICLE_MOVING || Utils::is_in_editor()) {
        return;
    }
    const Vector2 pos{get_position()};
    const Vector2 diff{navigation_target_ - pos};
    if (diff.length() <= destination_threshold_) {
        auto next{get_next_navigation_target_()};
        if (next.is_valid_target) {
            set_navigation_target(next.target);
        } else {
            auto tmp = navigation_target_;
            state_ = VEHICLE_IDLE;
            animated_sprite_->stop();
            map_route_inc_ = !map_route_inc_;
            navigation_target_ = Vector2();
            emit_signal("destination_reached", tmp);
        }
        return;
    }
    direction_ = diff.normalized();
    set_position(pos + (direction_ * speed_ * delta));
    update_animation_();
}

void godot::CL::TradingVehicle::start_navigating() {
    auto next{get_next_navigation_target_()};
    if (!next.is_valid_target) {
        // TODO err message
        return;
    }
    navigation_target_ = next.target;
    state_ = VEHICLE_MOVING;
}

void godot::CL::TradingVehicle::stop_navigating() {
    state_ = VEHICLE_IDLE;
    animated_sprite_->stop();
}

godot::CL::NextNavResult
godot::CL::TradingVehicle::get_next_navigation_target_() {
    int64_t route_size{map_route_.size()};
    NextNavResult result{};
    if (map_route_inc_) {
        if (current_map_route_idx_ < route_size) {
            result.target = map_route_[current_map_route_idx_];
            result.is_valid_target = result.target != navigation_target_;
            if (current_map_route_idx_ + 1 < route_size) {
                ++current_map_route_idx_;
            }
        }
    } else {
        if (current_map_route_idx_ >= 0) {
            result.target = map_route_[current_map_route_idx_];
            result.is_valid_target = result.target != navigation_target_;
            if (current_map_route_idx_ > 0) {
                --current_map_route_idx_;
            }
        }
    }
    return result;
}

void godot::CL::TradingVehicle::update_animation_() {
    const auto anim_idx{static_cast<int32_t>(
        (AnimationSize *
         (direction_.rotated(Math_PI / AnimationSize).angle() + Math_PI) /
         Math_TAU))};
    ERR_FAIL_INDEX_MSG(anim_idx, AnimationSize, "anim_idx is out of bounds");
    animated_sprite_->play(AnimationNames[anim_idx]);
}

void godot::CL::TradingVehicle::emit_debug_signal_() {
#ifdef CL_TRADING_DEBUG
    if (debug_mode_) {
        if (is_moving()) {
            emit_signal("draw_debug_lines", get_position(), navigation_target_);
        }
    } else {
        emit_signal("clear_debug_lines");
    }
#endif
}

void godot::CL::TradingVehicle::_ready() {
    if (Utils::is_in_editor()) {
        e_assign_required_components_();
    } else {
        r_assign_required_components_();
    }
    set_y_sort_enabled(true);
    set_z_index(6);
    emit_debug_signal_();
}

void godot::CL::TradingVehicle::_process(double delta) {
    if (Utils::is_in_game()) {
        handle_movement_(delta);
    }
}

void godot::CL::TradingVehicle::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_state"), &TradingVehicle::get_state);
    ClassDB::bind_method(D_METHOD("get_navigation_target"),
                         &TradingVehicle::get_navigation_target);

    ClassDB::bind_method(D_METHOD("set_map_path", "v"),
                         &TradingVehicle::set_map_path);
    ClassDB::bind_method(D_METHOD("clear_map_path"),
                         &TradingVehicle::clear_map_path);

    ClassDB::bind_method(D_METHOD("start_navigating"),
                         &TradingVehicle::start_navigating);
    ClassDB::bind_method(D_METHOD("stop_navigating"),
                         &TradingVehicle::stop_navigating);

    ClassDB::bind_method(D_METHOD("is_moving"), &TradingVehicle::is_moving);

    ClassDB::bind_method(D_METHOD("get_speed"), &TradingVehicle::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "s"),
                         &TradingVehicle::set_speed);

    ClassDB::bind_method(D_METHOD("get_tier"), &TradingVehicle::get_tier);
    ClassDB::bind_method(D_METHOD("set_tier", "t"), &TradingVehicle::set_tier);

    ClassDB::bind_method(D_METHOD("get_destination_threshold"),
                         &TradingVehicle::get_destination_threshold);
    ClassDB::bind_method(D_METHOD("set_destination_threshold", "t"),
                         &TradingVehicle::set_destination_threshold);

    ClassDB::add_property("TradingVehicle",
                          PropertyInfo(Variant::INT, "tier", PROPERTY_HINT_ENUM,
                                       "Budget,Common,Premium"),
                          "set_tier", "get_tier");

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
    ClassDB::add_signal(
        "TradingVehicle",
        MethodInfo("destination_reached",
                   PropertyInfo(Variant::VECTOR2, "destination")));

    // BIND ENUMS
    BIND_ENUM_CONSTANT(VEHICLE_IDLE);
    BIND_ENUM_CONSTANT(VEHICLE_MOVING);
    BIND_ENUM_CONSTANT(VEHICLE_ONLOADING);
    BIND_ENUM_CONSTANT(VEHICLE_OFFLOADING);

    BIND_ENUM_CONSTANT(VEHICLE_TIER_BUDGET);
    BIND_ENUM_CONSTANT(VEHICLE_TIER_COMMON);
    BIND_ENUM_CONSTANT(VEHICLE_TIER_PREMIUM);
}
