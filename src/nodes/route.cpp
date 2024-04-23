#include "route.h"

#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include "../core/utils.h"
#include "./trading_vehicle.h"

godot::CL::Route::Route()
    : initial_start_(true),
      kind_(ENTRYABLE_CITY),
      player_(""),
      current_route_(TypedArray<Vector2>()),
      type_(TILE_SURFACE_NONE),
      // timeout for time bewteen destination
      // reached and resuming of route
      timeout_cb_(Callable(this, "handle_timeout_")),
      dest_reached_cb_(Callable(this, "handle_destination_reached_")),
      start_(""),
      end_(""),
      state_(ROUTE_INACTIVE),
      cooldown_timer_(nullptr),
      distance_(0),
      gold_cost_(0),
      vehicle_(nullptr) {}

godot::CL::Route::~Route() { destroy(); }

void godot::CL::Route::destroy() {
    Utils::disconnect(vehicle_, TradingVehicle::SDestReached, dest_reached_cb_);
    Utils::disconnect(cooldown_timer_, "timeout", timeout_cb_);
    Utils::queue_delete(vehicle_);
    Utils::queue_delete(cooldown_timer_);

    vehicle_ = nullptr;
    cooldown_timer_ = nullptr;
    player_ = "";
    current_route_.clear();
    distance_ = 0;
    gold_cost_ = 0;
    type_ = TILE_SURFACE_NONE;
    state_ = ROUTE_INACTIVE;
    start_ = "";
    end_ = "";
}

void godot::CL::Route::handle_timeout_() {
    if (Utils::is_in_editor()) {
        return;
    }
    ERR_FAIL_NULL_MSG(vehicle_,
                      vformat("timeout: vehicle is null on %s", get_name()));
    vehicle_->start_navigating();
}

void godot::CL::Route::handle_destination_reached_(const Vector2 dest) {
    if (Utils::is_in_editor()) {
        return;
    }
    ERR_FAIL_NULL_MSG(cooldown_timer_,
                      vformat("dest_reached: timer is null %s", get_name()));
    cooldown_timer_->start();
}

bool godot::CL::Route::start() {
    ERR_FAIL_COND_V_MSG(current_route_.size() == 0, false,
                        "start: no route set");
    ERR_FAIL_NULL_V_MSG(vehicle_, false,
                        vformat("start: vehicle is null on %s", get_name()));
    if (initial_start_) {
        vehicle_->set_map_path(current_route_);
        vehicle_->set_position(current_route_[0]);
        initial_start_ = false;
    }
    vehicle_->start_navigating();
    state_ = ROUTE_ACTIVE;
    return true;
}

void godot::CL::Route::stop() {
    ERR_FAIL_NULL_MSG(vehicle_,
                      vformat("stop: vehicle_ is null on %s", get_name()));
    vehicle_->stop_navigating();
    state_ = ROUTE_INACTIVE;
}

void godot::CL::Route::set_vehicle(TradingVehicle *vehicle) {
    ERR_FAIL_NULL_MSG(
        vehicle, vformat("set_vehicle: new vehicle is null on %s", get_name()));
    auto *tmp = vehicle_;
    vehicle_ = vehicle;
    if (tmp != nullptr) {
        remove_child(tmp);
        Utils::queue_delete(tmp);
    }
    vehicle_->set_name(get_name() + String("Vehicle"));
    add_child(vehicle_);
    vehicle_->set_owner(this);
    Utils::connect(vehicle_, TradingVehicle::SDestReached, dest_reached_cb_);
}

void godot::CL::Route::setup_vehicle_from_tree_() {
    ERR_FAIL_COND_MSG(vehicle_ != nullptr,
                      vformat("vehicle is already assigned on %s", get_name()));
    Node *vehicle{find_child("*Vehicle")};
    if (vehicle == nullptr) {
        return;
    }
    vehicle_ = static_cast<TradingVehicle *>(vehicle);
    if (vehicle_ == nullptr) {
        std::cout << "Vehicle is nullptr\n";
    }
    Utils::connect(vehicle_, TradingVehicle::SDestReached, dest_reached_cb_);
}

void godot::CL::Route::setup_timer_from_tree_or_create_() {
    ERR_FAIL_COND_MSG(cooldown_timer_ != nullptr,
                      vformat("timer is already assigned on %s", get_name()));
    Node *timer_node{find_child("*Timer*")};
    if (timer_node != nullptr) {
        cooldown_timer_ = static_cast<Timer *>(timer_node);
    } else {
        cooldown_timer_ = memnew(Timer);
        cooldown_timer_->set_one_shot(true);
        add_child(cooldown_timer_);
        cooldown_timer_->set_owner(this);
    }
    // TODO make variable
    cooldown_timer_->set_wait_time(5.0);
    Utils::connect(cooldown_timer_, "timeout",
                   Callable(this, "handle_timeout_"));
}

void godot::CL::Route::_ready() {
    if (Utils::is_in_editor()) {
        return;
    }
    setup_timer_from_tree_or_create_();
    setup_vehicle_from_tree_();
}

void godot::CL::Route::_exit_tree() {
    if (Utils::is_in_editor()) {
        return;
    }
    destroy();
}

void godot::CL::Route::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("handle_timeout_"), &Route::handle_timeout_);
    ClassDB::bind_method(D_METHOD("handle_destination_reached_", "dest"),
                         &Route::handle_destination_reached_);

    ClassDB::bind_method(D_METHOD("get_current_route"),
                         &Route::get_current_route);
    ClassDB::bind_method(D_METHOD("set_current_route", "path"),
                         &Route::set_current_route);

    ClassDB::bind_method(D_METHOD("start"), &Route::start);
    ClassDB::bind_method(D_METHOD("stop"), &Route::stop);
    ClassDB::bind_method(D_METHOD("get_vehicle"), &Route::get_vehicle);
    ClassDB::bind_method(D_METHOD("set_vehicle", "vehicle"),
                         &Route::set_vehicle);

    ClassDB::bind_method(D_METHOD("get_start"), &Route::get_start);
    ClassDB::bind_method(D_METHOD("set_start", "name"), &Route::set_start);
    ClassDB::bind_method(D_METHOD("get_end"), &Route::get_end);
    ClassDB::bind_method(D_METHOD("set_end", "name"), &Route::set_end);
    ClassDB::bind_method(D_METHOD("get_player"), &Route::get_player);
    ClassDB::bind_method(D_METHOD("set_player", "p"), &Route::set_player);

    ClassDB::bind_method(D_METHOD("get_type"), &Route::get_type);
    ClassDB::bind_method(D_METHOD("set_type", "t"), &Route::set_type);

    ClassDB::bind_method(D_METHOD("get_kind"), &Route::get_kind);
    ClassDB::bind_method(D_METHOD("set_kind", "k"), &Route::set_kind);

    // BIND ENUMS
    BIND_ENUM_CONSTANT(ROUTE_INACTIVE);
    BIND_ENUM_CONSTANT(ROUTE_ACTIVE);
}
