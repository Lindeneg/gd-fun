#include "route_manager.h"

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../core/entryable.h"
#include "../core/utils.h"
#include "./city_manager.h"
#include "./resource_manager.h"
#include "./trading_vehicle.h"

#ifdef CL_TRADING_DEBUG
MAKE_M_LOG(ROUTEMLOG, RouteManager)
#endif

godot::CL::RouteManager::RouteManager()
    : debug_(false),
      routes_(Dictionary()),
      city_manager_(nullptr),
      resource_manager_(nullptr),
      offload_cargo_cb_(Callable(this, "handle_offload_cargo_")),
      onload_cargo_cb_(Callable(this, "handle_onload_cargo_")),
      onload_finished_cb_(Callable(this, "handle_onload_finished_")),
      offload_finished_cb_(Callable(this, "handle_offload_finished_")) {}

godot::CL::RouteManager::~RouteManager() {}

void godot::CL::RouteManager::handle_offload_cargo_(StringName player_name,
                                                    StringName route_name,
                                                    ResourceKind kind) {
    Route *route{get_player_route(player_name, route_name)};
    ERR_FAIL_NULL(route);
    StringName city_name{route->get_target_entryable()};
    City *city{city_manager_->get_city(city_name)};
    ERR_FAIL_NULL(city);
    auto offloaded{city->receive_resource(kind, 1)};
    if (offloaded.amount > 0) {
        route->consume_current_resource(offloaded.amount);
        if (offloaded.accepted_amount > 0) {
            // TODO: NOTIFY FINANCE
        }
    } else {
        route->go_to_next_cargo();
    }
    route->queue_offload_cargo();
}

void godot::CL::RouteManager::handle_onload_cargo_(StringName player_name,
                                                   StringName route_name,
                                                   ResourceKind kind) {
    Route *route{get_player_route(player_name, route_name)};
    ERR_FAIL_NULL(route);
    if (route->get_kind() == ENTRYABLE_CITY) {
        StringName city_name{route->get_target_entryable_inv()};
        City *city{city_manager_->get_city(city_name)};
        ERR_FAIL_NULL(city);
        int consumed{city->consume_resource(kind, 1)};
        if (consumed > 0) {
            route->receive_current_resource(consumed);
        } else {
            route->go_to_next_cargo();
        }
    } else {
        ResourceTile *res{resource_manager_->get_resource(route->get_end())};
        ERR_FAIL_NULL(res);
        int current_amount{res->get_current_amount()};
        if (current_amount > 0) {
            res->set_current_amount(current_amount - 1);
            route->receive_current_resource(1);
        } else {
            handle_onload_finished_(player_name, route_name);
            return;
        }
    }
    route->queue_onload_cargo();
}

void godot::CL::RouteManager::handle_offload_finished_(StringName player_name,
                                                       StringName route_name) {
    Route *route{get_player_route(player_name, route_name)};
    ERR_FAIL_NULL(route);
#ifdef CL_TRADING_DEBUG
    ROUTEMLOG(
        debug_, "%s\n",
        GDSTR(vformat("%s finished offloading cargo", route->get_name())));
#endif
    route->get_vehicle()->set_state(VEHICLE_ONLOADING);
    route->switch_dir();
    route->queue_onload_cargo();
}

void godot::CL::RouteManager::handle_onload_finished_(StringName player_name,
                                                      StringName route_name) {
    Route *route{get_player_route(player_name, route_name)};
    ERR_FAIL_NULL(route);
#ifdef CL_TRADING_DEBUG
    ROUTEMLOG(debug_, "%s\n",
              GDSTR(vformat("%s finished onloading cargo", route->get_name())));
#endif
    route->start();
}

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
#ifdef CL_TRADING_DEBUG
    ROUTEMLOG(debug_, "%s\n",
              GDSTR(vformat("created route %s for player %s, total %d",
                            route->get_name(), player_name, routes.size())));
#endif
    player->add_connection(route->get_start());
    player->add_connection(route->get_end());
    Utils::connect(route, Route::SOnloadCargo, onload_cargo_cb_);
    Utils::connect(route, Route::SOffloadCargo, offload_cargo_cb_);
    Utils::connect(route, Route::SOffloadCargoFinished, offload_finished_cb_);
    Utils::connect(route, Route::SOnloadCargoFinished, onload_finished_cb_);
    route->start();
}

void godot::CL::RouteManager::remove_route_from_name(
    const StringName player_name, const StringName route_name) {
    Route *route{get_player_route(player_name, route_name)};
    remove_route(route);
}

void godot::CL::RouteManager::remove_route(Route *route) {
    ERR_FAIL_NULL(route);
    route->stop();
    Player *player{route->get_player()};
    ERR_FAIL_NULL(player);
    StringName player_name{player->get_name()};
    auto routes{static_cast<TypedArray<Route>>(
        routes_.get(player_name, TypedArray<Route>{}))};
    routes.erase(route);
    routes_[player_name] = routes;
#ifdef CL_TRADING_DEBUG
    ROUTEMLOG(debug_, "%s\n",
              GDSTR(vformat("erased route %s for player %s, total %d",
                            route->get_name(), player_name, routes.size())));
#endif
    player->remove_connection(route->get_start());
    player->remove_connection(route->get_end());
    Utils::disconnect(route, Route::SOnloadCargo, onload_cargo_cb_);
    Utils::disconnect(route, Route::SOffloadCargo, offload_cargo_cb_);
    Utils::disconnect(route, Route::SOffloadCargoFinished,
                      offload_finished_cb_);
    Utils::disconnect(route, Route::SOnloadCargoFinished, onload_finished_cb_);
    Utils::queue_delete(route);
}

void godot::CL::RouteManager::_bind_methods() {
    // BIND METHODS
    DEBUG_BIND(RouteManager)

    ClassDB::bind_method(D_METHOD("add_route", "r"), &RouteManager::add_route);
    ClassDB::bind_method(D_METHOD("remove_route", "r"),
                         &RouteManager::remove_route);
    ClassDB::bind_method(D_METHOD("remove_route_from_name", "p", "r"),
                         &RouteManager::remove_route_from_name);
    ClassDB::bind_method(D_METHOD("get_player_route", "n", "r"),
                         &RouteManager::get_player_route);
    ClassDB::bind_method(D_METHOD("get_player_routes", "n"),
                         &RouteManager::get_player_routes);

    ClassDB::bind_method(D_METHOD("handle_offload_cargo_", "p", "r", "k"),
                         &RouteManager::handle_offload_cargo_);
    ClassDB::bind_method(D_METHOD("handle_onload_cargo_", "p", "r", "k"),
                         &RouteManager::handle_onload_cargo_);
    ClassDB::bind_method(D_METHOD("handle_offload_finished_", "p", "r"),
                         &RouteManager::handle_offload_finished_);
    ClassDB::bind_method(D_METHOD("handle_onload_finished_", "p", "r"),
                         &RouteManager::handle_onload_finished_);
}
