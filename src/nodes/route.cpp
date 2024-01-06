#include "route.h"

#include <godot_cpp/core/class_db.hpp>
#include <iostream>

#include "./trading_vehicle.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/resource_preloader.hpp"

godot::CL::Route::Route()
    : c1_(nullptr),
      c2_(nullptr),
      distance_(0),
      gold_cost_(0),
      vehicle_(nullptr) {}

godot::CL::Route::~Route() {
    if (vehicle_ != nullptr) {
#ifdef CL_TRADING_DEBUG
        std::cout << "Route: Freeing TradingVehicle\n";
#endif
        vehicle_->queue_free();
    }
    vehicle_ = nullptr;
}

void godot::CL::Route::set_vehicle(TradingVehicle* vehicle) {
    if (vehicle_ != nullptr) {
        vehicle_->queue_free();
        vehicle_ = nullptr;
    }
    vehicle_ = vehicle;
}

void godot::CL::Route::change_route_plan() {}
void godot::CL::Route::change_trading_vehicle() {}

void godot::CL::Route::_ready() {
    //    auto res = godot::ResourceLoader().load("res://sprites/ship.tscn");
    //    if (res == nullptr) {
    //        return;
    //    }
    //    auto k = cast_to<PackedScene>(*res);
    //    if (k == nullptr) {
    //        return;
    //    }
    //    k->initialize_class();
    //    auto instance = k->instantiate();
    //    add_child(instance);
}

void godot::CL::Route::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_vehicle"), &Route::get_vehicle);
    ClassDB::bind_method(D_METHOD("set_vehicle", "vehicle"),
                         &Route::set_vehicle);

    BIND_ENUM_CONSTANT(ROUTE_INACTIVE);
    BIND_ENUM_CONSTANT(ROUTE_ACTIVE);
}
