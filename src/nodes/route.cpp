#include "route.h"

#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include "../core/utils.h"
#include "./player_finance.h"
#include "./trading_vehicle.h"

#ifdef CL_TRADING_DEBUG
MAKE_LOG(ROUTELOG, Route)
#endif

const char *godot::CL::Route::SOnloadCargo{"onload-cargo"};
const char *godot::CL::Route::SOffloadCargo{"offload-cargo"};
const char *godot::CL::Route::SOnloadCargoFinished{"onload-finished"};
const char *godot::CL::Route::SOffloadCargoFinished{"offload-finished"};

godot::CL::Route::Route()
    : debug_(false),
      initial_start_(true),
      kind_(ENTRYABLE_CITY),
      player_(nullptr),
      current_route_(TypedArray<Vector2>()),
      cargo_(Dictionary()),
      type_(TILE_SURFACE_NONE),
      timeout_cb_(Callable(this, "handle_timeout_")),
      dest_reached_cb_(Callable(this, "handle_destination_reached_")),
      upkeep_required_cb_(Callable(this, "handle_upkeep_required_")),
      start_(""),
      end_(""),
      state_(ROUTE_INACTIVE),
      cooldown_timer_(nullptr),
      total_profits_(0),
      distance_(0),
      current_cargo_idx_(0),
      gold_cost_(0),
      vehicle_(nullptr) {}

godot::CL::Route::~Route() {
#ifdef CL_TRADING_DEBUG
    ROUTELOG(this, "destructor called\n");
#endif
    destroy();
}

void godot::CL::Route::destroy() {
    Utils::disconnect(vehicle_, TradingVehicle::SDestReached, dest_reached_cb_);
    Utils::disconnect(vehicle_, TradingVehicle::SUpkeepRequired,
                      upkeep_required_cb_);
    Utils::disconnect(cooldown_timer_, "timeout", timeout_cb_);
    Utils::queue_delete(vehicle_);
    Utils::queue_delete(cooldown_timer_);

    vehicle_ = nullptr;
    cooldown_timer_ = nullptr;
    player_ = nullptr;
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
    auto current_cargo{get_current_cargo()};
    auto cargo{cast_to<CityResource>(current_cargo[current_cargo_idx_])};
    if (vehicle_->get_state() == VEHICLE_OFFLOADING) {
        emit_signal(SOffloadCargo, player_->get_name(), get_name(),
                    cargo->get_resource_kind());
    } else if (vehicle_->get_state() == VEHICLE_ONLOADING) {
        emit_signal(SOnloadCargo, player_->get_name(), get_name(),
                    cargo->get_resource_kind());
    }
}

void godot::CL::Route::handle_upkeep_required_() {
    if (Utils::is_in_editor()) {
        return;
    }
    int upkeep{vehicle_->get_upkeep()};
    player_->get_finance()->add_expense(FINANCE_SUB_KIND_UPKEEP, get_name(),
                                        upkeep, 1);
    add_to_total_profits(-upkeep);
}

void godot::CL::Route::handle_destination_reached_(const int direction) {
    if (Utils::is_in_editor()) {
        return;
    }
#ifdef CL_TRADING_DEBUG
    ROUTELOG(this, "destination reached with direction %d\n", direction);
#endif
    current_cargo_idx_ = 0;
    vehicle_->set_state(VEHICLE_OFFLOADING);
    queue_offload_cargo();
}

void godot::CL::Route::queue_offload_cargo() {
    ERR_FAIL_COND(vehicle_->get_state() != VEHICLE_OFFLOADING);
    auto current_cargo{get_current_cargo()};
    if (current_cargo_idx_ >= current_cargo.size()) {
        emit_signal(SOffloadCargoFinished, player_->get_name(), get_name());
        return;
    }
    auto cargo{cast_to<CityResource>(current_cargo[current_cargo_idx_])};
    if (cargo->get_amount() > 0) {
        cooldown_timer_->start();
    } else {
        current_cargo_idx_++;
        queue_offload_cargo();
    }
}

void godot::CL::Route::queue_onload_cargo() {
    ERR_FAIL_COND(vehicle_->get_state() != VEHICLE_ONLOADING);
    auto current_cargo{get_current_cargo()};
    if (current_cargo_idx_ >= current_cargo.size()) {
        emit_signal(SOnloadCargoFinished, player_->get_name(), get_name());
        return;
    }
    auto cargo{cast_to<CityResource>(current_cargo[current_cargo_idx_])};
    if (cargo->get_amount() < cargo->get_capacity()) {
        cooldown_timer_->start();
    } else {
        current_cargo_idx_++;
        queue_onload_cargo();
    }
}

void godot::CL::Route::receive_current_resource(int amount) {
    ERR_FAIL_COND(vehicle_->get_state() != VEHICLE_ONLOADING);
    auto current_cargo{get_current_cargo()};
    auto cargo{cast_to<CityResource>(current_cargo[current_cargo_idx_])};
    int total{cargo->get_amount() + amount};
    ERR_FAIL_COND(total > cargo->get_capacity());
    cargo->set_amount(total);
#ifdef CL_TRADING_DEBUG
    ROUTELOG(this, "onloaded resource %d for total %d\n",
             cargo->get_resource_kind(), total);
#endif
}

void godot::CL::Route::consume_current_resource(int amount) {
    ERR_FAIL_COND(vehicle_->get_state() != VEHICLE_OFFLOADING);
    auto current_cargo{get_current_cargo()};
    auto cargo{cast_to<CityResource>(current_cargo[current_cargo_idx_])};
    int total{cargo->get_amount() - amount};
    ERR_FAIL_COND(total < 0);
    cargo->set_amount(total);
#ifdef CL_TRADING_DEBUG
    ROUTELOG(this, "offloaded resource %d for total %d\n",
             cargo->get_resource_kind(), total);
#endif
}

void godot::CL::Route::go_to_next_cargo() { current_cargo_idx_++; }

void godot::CL::Route::switch_dir() {
    current_cargo_idx_ = 0;
    vehicle_->switch_move_dir();
}

void godot::CL::Route::start() {
    ERR_FAIL_COND_MSG(current_route_.size() == 0, "start: no route set");
    ERR_FAIL_NULL_MSG(vehicle_,
                      vformat("start: vehicle is null on %s", get_name()));
    if (initial_start_) {
        vehicle_->set_map_path(current_route_);
        vehicle_->set_position(current_route_[0]);
        vehicle_->start_upkeep_timer();
        initial_start_ = false;
    }
    vehicle_->start_navigating();
    state_ = ROUTE_ACTIVE;
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
    Utils::connect(vehicle_, TradingVehicle::SUpkeepRequired,
                   upkeep_required_cb_);
}

void godot::CL::Route::setup_vehicle_from_tree_() {
    if (vehicle_ != nullptr) {
        return;
    }
    Node *vehicle{find_child("*Vehicle")};
    if (vehicle == nullptr) {
        return;
    }
    vehicle_ = static_cast<TradingVehicle *>(vehicle);
    ERR_FAIL_NULL(vehicle);
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
    cooldown_timer_->set_wait_time(1.5f);
    Utils::connect(cooldown_timer_, "timeout",
                   Callable(this, "handle_timeout_"));
}

godot::TypedArray<godot::CL::CityResource> godot::CL::Route::get_current_cargo()
    const {
    return cargo_[vehicle_->get_move_dir()];
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
    DEBUG_BIND(Route);

    ClassDB::bind_method(D_METHOD("handle_timeout_"), &Route::handle_timeout_);
    ClassDB::bind_method(D_METHOD("handle_upkeep_required_"),
                         &Route::handle_upkeep_required_);
    ClassDB::bind_method(D_METHOD("handle_destination_reached_", "dest"),
                         &Route::handle_destination_reached_);

    ClassDB::bind_method(D_METHOD("get_current_route"),
                         &Route::get_current_route);
    ClassDB::bind_method(D_METHOD("set_current_route", "path"),
                         &Route::set_current_route);

    ClassDB::bind_method(D_METHOD("get_total_profits"),
                         &Route::get_total_profits);
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

    ClassDB::bind_method(D_METHOD("get_cargo"), &Route::get_cargo);
    ClassDB::bind_method(D_METHOD("set_cargo", "c"), &Route::set_cargo);

    ClassDB::bind_method(D_METHOD("get_target_entryable"),
                         &Route::get_target_entryable);

    ClassDB::bind_method(D_METHOD("get_current_cargo"),
                         &Route::get_current_cargo);

    ClassDB::bind_method(D_METHOD("set_timeout_duration", "i"),
                         &Route::set_timeout_duration);
    ClassDB::bind_method(D_METHOD("start_timer"), &Route::start_timer);
    ClassDB::bind_method(D_METHOD("stop_timer"), &Route::stop_timer);

    // SIGNALS
    ClassDB::add_signal(
        "Route", MethodInfo(SOnloadCargo,
                            PropertyInfo(Variant::STRING_NAME, "player_name"),
                            PropertyInfo(Variant::STRING_NAME, "route_name"),
                            PropertyInfo(Variant::INT, "kind")));
    ClassDB::add_signal(
        "Route", MethodInfo(SOffloadCargo,
                            PropertyInfo(Variant::STRING_NAME, "player_name"),
                            PropertyInfo(Variant::STRING_NAME, "route_name"),
                            PropertyInfo(Variant::INT, "kind")));
    ClassDB::add_signal(
        "Route", MethodInfo(SOffloadCargoFinished,
                            PropertyInfo(Variant::STRING_NAME, "player_name"),
                            PropertyInfo(Variant::STRING_NAME, "route_name")));
    ClassDB::add_signal(
        "Route", MethodInfo(SOnloadCargoFinished,
                            PropertyInfo(Variant::STRING_NAME, "player_name"),
                            PropertyInfo(Variant::STRING_NAME, "route_name")));

    // BIND ENUMS
    BIND_ENUM_CONSTANT(ROUTE_INACTIVE);
    BIND_ENUM_CONSTANT(ROUTE_ACTIVE);
}
