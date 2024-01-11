#include "route.h"

#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include "../core/utils.h"
#include "./city_manager.h"
#include "./tile_manager.h"
#include "./trading_vehicle.h"

godot::CL::Route::Route()
    : debug_mode_(false),
      type_(TILE_SURFACE_NONE),
      tile_manager_(nullptr),
      city_manager_(nullptr),
      timeout_cb_(Callable(this, "handle_timeout_")),
      dest_reached_cb_(Callable(this, "handle_destination_reached_")),
      c1_(""),
      c2_(""),
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
    tile_manager_ = nullptr;
    city_manager_ = nullptr;
    distance_ = 0;
    gold_cost_ = 0;
    type_ = TILE_SURFACE_NONE;
    state_ = ROUTE_INACTIVE;
    c1_ = "";
    c2_ = "";
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

void godot::CL::Route::set_debug_mode(const bool m) {
    debug_mode_ = m;
    emit_debug_signal_();
}

void godot::CL::Route::emit_debug_signal_() {
#ifdef CL_TRADING_DEBUG
    if (debug_mode_ && current_route_.size() > 0) {
        emit_signal("draw_debug_route", get_name(), current_route_);
    } else {
        emit_signal("remove_debug_route", get_name());
    }
#endif
}

godot::Vector2 godot::CL::Route::get_city_entry_(StringName city_name) const {
    City* city = city_manager_->get_city(city_name);
    ERR_FAIL_NULL_V_MSG(city, Vector2(),
                        vformat("get_city_entry: city %s is null on %s",
                                city_name, get_name()));

    auto entries{type_ == TILE_SURFACE_GROUND ? city->get_onshore_entries()
                                              : city->get_offshore_entries()};
    ERR_FAIL_COND_V_MSG(entries.size() == 0, Vector2(),
                        vformat("get_city_entry: city %s has no entries on %s",
                                city_name, get_name()));
    // TODO perhaps pick the closet point
    return entries[0];
}

godot::TypedArray<godot::Vector2> godot::CL::Route::get_local_path_() {
    TypedArray<Vector2> result{};
    auto size = current_route_.size();
    for (int i = 0; i < size; i++) {
        result.append(tile_manager_->map_to_local(current_route_[i]));
    }
    return result;
}

bool godot::CL::Route::start(const bool initial_start) {
    ERR_FAIL_NULL_V_MSG(vehicle_, false,
                        vformat("start: vehicle is null on %s", get_name()));
    ERR_FAIL_COND_V_MSG(
        !has_required_managers_(), false,
        vformat("start: required managers missing on %s", get_name()));
    // TODO(5) probably dont take bool as arg
    // rather just keep that state internal
    if (initial_start) {
        current_route_ = tile_manager_->construct_path(
            get_city_entry_(c1_), get_city_entry_(c2_), type_);
        auto local_path = get_local_path_();
        ERR_FAIL_COND_V_MSG(
            local_path.size() == 0, false,
            vformat("start: could not calculate path on %s", get_name()));
        vehicle_->set_map_path(local_path);
        vehicle_->set_position(local_path[0]);
    }
    vehicle_->start_navigating();
    state_ = ROUTE_ACTIVE;
    cooldown_timer_->start();
    emit_debug_signal_();
    return true;
}

void godot::CL::Route::stop() {
    ERR_FAIL_NULL_MSG(vehicle_,
                      vformat("stop: vehicle_ is null on %s", get_name()));
    vehicle_->stop_navigating();
    state_ = ROUTE_INACTIVE;
}

void godot::CL::Route::set_vehicle(TradingVehicle* vehicle) {
    ERR_FAIL_NULL_MSG(
        vehicle, vformat("set_vehicle: new vehicle is null on %s", get_name()));
    auto* tmp = vehicle_;
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

void godot::CL::Route::change_route_plan() {}
void godot::CL::Route::change_trading_vehicle() {}

void godot::CL::Route::setup_vehicle_from_tree_() {
    ERR_FAIL_COND_MSG(vehicle_ != nullptr,
                      vformat("vehicle is already assigned on %s", get_name()));
    Node* vehicle{find_child("*Vehicle")};
    if (vehicle == nullptr) {
        return;
    }
    vehicle_ = static_cast<TradingVehicle*>(vehicle);
    if (vehicle_ == nullptr) {
        std::cout << "Vehicle is nullptr\n";
    }
    Utils::connect(vehicle_, TradingVehicle::SDestReached, dest_reached_cb_);
}

void godot::CL::Route::setup_timer_from_tree_or_create_() {
    ERR_FAIL_COND_MSG(cooldown_timer_ != nullptr,
                      vformat("timer is already assigned on %s", get_name()));
    Node* timer_node{find_child("*Timer*")};
    if (timer_node != nullptr) {
        cooldown_timer_ = static_cast<Timer*>(timer_node);
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

bool godot::CL::Route::has_required_managers_() const {
    return tile_manager_ != nullptr && city_manager_ != nullptr;
}

void godot::CL::Route::_ready() {
    if (Utils::is_in_editor()) {
        return;
    }
    setup_timer_from_tree_or_create_();
    setup_vehicle_from_tree_();
}

void godot::CL::Route::_enter_tree() {
    if (Utils::is_in_editor()) {
        return;
    }
    auto* tile_manager = get_node_or_null(NodePath("../../TileManager"));
    auto* city_manager = get_node_or_null(NodePath("../../CityManager"));
    if (tile_manager != nullptr) {
        tile_manager_ = static_cast<TileManager*>(tile_manager);
    }
    if (city_manager != nullptr) {
        city_manager_ = static_cast<CityManager*>(city_manager);
    }
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

    ClassDB::bind_method(D_METHOD("start", "initial_start"), &Route::start);
    ClassDB::bind_method(D_METHOD("stop"), &Route::stop);
    ClassDB::bind_method(D_METHOD("get_vehicle"), &Route::get_vehicle);
    ClassDB::bind_method(D_METHOD("set_vehicle", "vehicle"),
                         &Route::set_vehicle);

    ClassDB::bind_method(D_METHOD("get_city_one"), &Route::get_city_one);
    ClassDB::bind_method(D_METHOD("set_city_one", "id"), &Route::set_city_one);
    ClassDB::bind_method(D_METHOD("get_city_two"), &Route::get_city_two);
    ClassDB::bind_method(D_METHOD("set_city_two", "id"), &Route::set_city_two);

    ClassDB::bind_method(D_METHOD("get_type"), &Route::get_type);
    ClassDB::bind_method(D_METHOD("set_type", "t"), &Route::set_type);

    ClassDB::bind_method(D_METHOD("get_debug_mode"), &Route::get_debug_mode);
    ClassDB::bind_method(D_METHOD("set_debug_mode", "m"),
                         &Route::set_debug_mode);

    // ADD SIGNALS
    ClassDB::add_signal("Route",
                        MethodInfo("draw_debug_route",
                                   PropertyInfo(Variant::STRING, "route_name"),
                                   PropertyInfo(Variant::ARRAY, "route")));
    ClassDB::add_signal(
        "Route", MethodInfo("remove_debug_route",
                            PropertyInfo(Variant::STRING, "route_name")));

    // BIND ENUMS
    BIND_ENUM_CONSTANT(ROUTE_INACTIVE);
    BIND_ENUM_CONSTANT(ROUTE_ACTIVE);
}
