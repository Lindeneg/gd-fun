#include "route.h"

#include <godot_cpp/core/class_db.hpp>
#include <iostream>

#include "./trading_vehicle.h"

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

// TODO
void godot::CL::Route::change_route_plan() {}
// TODO
void godot::CL::Route::change_trading_vehicle() {}

void godot::CL::Route::_bind_methods() {
    BIND_ENUM_CONSTANT(ROUTE_INACTIVE);
    BIND_ENUM_CONSTANT(ROUTE_ACTIVE);
}
