#include "route_manager.h"

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../core/utils.h"
#include "./city_manager.h"
#include "./resource.h"
#include "./resource_manager.h"
#include "./trading_vehicle.h"

namespace godot::CL {
void ROUTEMLOG NEW_M_LOG(RouteManager)
}  // namespace godot::CL

godot::CL::RouteManager::RouteManager()
    : routes_(Dictionary()),
      city_manager_(nullptr),
      resource_manager_(nullptr) {}

godot::CL::RouteManager::~RouteManager() {}

void godot::CL::RouteManager::handle_dest_reached_(StringName player_name,
                                                   StringName route_name,
                                                   VehicleMoveDir direction) {
    Route *route{get_player_route(player_name, route_name)};
    ERR_FAIL_NULL(route);
    EntryableKind route_kind{route->get_kind()};
    if (route_kind == ENTRYABLE_CITY) {
        handle_route_city_dest_(route);
    } else if (route_kind == ENTRYABLE_RESOURCE) {
        handle_route_resource_dest_(route, direction);
    }
    route->start();
}

void godot::CL::RouteManager::handle_route_city_dest_(Route *route) {
    StringName city_name{route->get_target_entryable()};
    City *city{city_manager_->get_city(city_name)};
    ERR_FAIL_NULL(city);
    auto offload_cargo{route->get_current_cargo()};
    for (int i = 0; i < offload_cargo.size(); i++) {
        auto cargo{cast_to<CityResource>(offload_cargo[i])};
        if (cargo->get_amount() <= 0) {
            continue;
        }
        // TODO notify finance stuff
        // TODO have a delay before offloading
        auto kind{cargo->get_resource_kind()};
        int offloaded{city->receive_resource(kind, cargo->get_amount())};
#ifdef CL_TRADING_ROUTE_DEBUG
        ROUTEMLOG("%s\n", GDSTR(vformat("%s offloaded %d of kind %d to city %s",
                                        route->get_name(), offloaded, kind,
                                        city_name)));
#endif
        cargo->set_amount(0);
    }
    route->get_vehicle()->switch_move_dir();
    auto onload_cargo{route->get_current_cargo()};
    for (int i = 0; i < onload_cargo.size(); i++) {
        auto cargo{cast_to<CityResource>(onload_cargo[i])};
        if (cargo->get_capacity() <= 0) {
            continue;
        }
        // TODO have a delay before onloading
        auto kind{cargo->get_resource_kind()};
        int onloaded{city->consume_resource(kind, cargo->get_capacity())};
#ifdef CL_TRADING_ROUTE_DEBUG
        if (onloaded > 0) {
            ROUTEMLOG(
                "%s\n",
                GDSTR(vformat("%s onloaded %d of kind %d from city %s",
                              route->get_name(), onloaded, kind, city_name)));
        }
#endif
        cargo->set_amount(onloaded);
    }
}
void godot::CL::RouteManager::handle_route_resource_dest_(
    Route *route, VehicleMoveDir direction) {
//    if (direction == VEHICLE_MOVE_DIR_AB) {
//        ResourceTile *res{resource_manager_->get_resource(route->get_end())};
//        ERR_FAIL_NULL(res);
//        int total_amount{res->get_current_amount()};
//        if (total_amount <= 0) {
//            return;
//        }
//        auto onload_cargo{route->get_current_cargo()};
//        // TODO handle resource onload
//    } else {
//        // TODO handle resource offload
//    }
    route->get_vehicle()->switch_move_dir();
}

void godot::CL::RouteManager::handle_timeout_(StringName player_name,
                                              StringName route_name) {}

void godot::CL::RouteManager::_enter_tree() {
    if (Utils::is_in_editor()) return;

    Node *city_node{get_node_or_null("../CityManager")};
    ERR_FAIL_NULL_MSG(city_node, "CityManager not found in tree");
    city_manager_ = static_cast<CityManager *>(city_node);

    Node *resource_node{get_node_or_null("../ResourceManager")};
    ERR_FAIL_NULL_MSG(resource_node, "ResourceManager not found in tree");
    resource_manager_ = static_cast<ResourceManager *>(resource_node);
}

godot::TypedArray<godot::CL::Route> godot::CL::RouteManager::get_player_routes(
    const StringName player_name) const {
    if (routes_.has(player_name)) {
        return static_cast<TypedArray<Route>>(routes_[player_name]);
    }
    return TypedArray<Route>{};
}

godot::CL::Route *godot::CL::RouteManager::get_player_route(
    const StringName player_name, const StringName route_name) const {
    if (!routes_.has(player_name)) {
        return nullptr;
    }
    auto player_routes{static_cast<TypedArray<Route>>(routes_[player_name])};
    for (int i = 0; i < player_routes.size(); i++) {
        Route *route{cast_to<Route>(player_routes[i])};
        if (route->get_name() == route_name) {
            return route;
        }
    }
    return nullptr;
}

void godot::CL::RouteManager::add_route(Route *route) {
    add_child(route);
    Player *player{route->get_player()};
    StringName player_name{player->get_name()};
    auto routes{static_cast<TypedArray<Route>>(
        routes_.get(player_name, TypedArray<Route>{}))};
    routes.append(route);
    routes_[player_name] = routes;
#ifdef CL_TRADING_ROUTE_DEBUG
    ROUTEMLOG("%s\n",
              GDSTR(vformat("created route %s for player %s, total %d",
                            route->get_name(), player_name, routes.size())));
#endif
    player->add_connection(route->get_start());
    player->add_connection(route->get_end());
    // TODO disconnect on route removal
    route->connect(TradingVehicle::SDestReached,
                   Callable(this, "handle_dest_reached_"));
    route->connect("timeout", Callable(this, "handle_timeout_"));
    route->start();
}

void godot::CL::RouteManager::remove_route_from_name(const StringName name) {}

void godot::CL::RouteManager::remove_route(const Route *route) {}

void godot::CL::RouteManager::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("handle_dest_reached_", "p", "r", "d"),
                         &RouteManager::handle_dest_reached_);
    ClassDB::bind_method(D_METHOD("handle_timeout_", "p", "r"),
                         &RouteManager::handle_timeout_);

    ClassDB::bind_method(D_METHOD("add_route", "r"), &RouteManager::add_route);
    ClassDB::bind_method(D_METHOD("remove_route", "r"),
                         &RouteManager::remove_route);
    ClassDB::bind_method(D_METHOD("remove_route_from_name", "n"),
                         &RouteManager::remove_route_from_name);
    ClassDB::bind_method(D_METHOD("get_player_route", "n", "r"),
                         &RouteManager::get_player_route);
    ClassDB::bind_method(D_METHOD("get_player_routes", "n"),
                         &RouteManager::get_player_routes);
}
