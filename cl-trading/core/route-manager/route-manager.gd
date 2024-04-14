"""
RouteManager knows everything about how to create, update and delete Routes.
"""
class_name RouteManager extends Node

# CONSTANTS

# Dictionaries with Vehicle information
# IMPORTANT:
# add here when you create new vehicle scenes
# NOTE(cl): we can probably find a better way to do it
const VEHICLES = {
	"ONSHORE": {
		"HORSE": preload("res://sprites/vehicles/onshore/vehicle_horse.tscn"),
	},
	"OFFSHORE": {
		"SHIP": preload("res://sprites/vehicles/offshore/vehicle_ship.tscn")
	}
};

# CORE FUNCTIONS

# this function will be called from UI after a player
# has configured a Route and wants to buy it
func create_and_init_route(
	city1: String,
	city2: String,
	vehicle_type: String,
	vehicle_name: String,
	surface: int
) -> void:
	var route: Route = Route.new();
	var vehicle = instanciate_vehicle(vehicle_type, vehicle_name);
	if !vehicle:
		printerr("failed to create route {c1}<->{c2} with vehicle {v} - {t}".format(
			{"c1": city1, "c2": city2, "v": vehicle_name, "t": vehicle_type}
		));
		return;

	add_child(route);

	route.set_type(surface);
	route.set_kind(Route.ROUTE_CITY_RESOURCE);
	route.set_start(city1);
	route.set_end(city2);
	route.set_vehicle(vehicle);

	var did_start = route.start();
	if !did_start:
		remove_child(route);
		return;

# Instanciates a new instance of
# TradingVehicle given a type and a name
func instanciate_vehicle(vehicle_type: String, vehicle_name: String) -> TradingVehicle:
	var type: Dictionary = VEHICLES.get(vehicle_type);
	if !type:
		printerr("could not find vehicle_type: {t}".format({"t": vehicle_type}));
		return null;
	var vehicle_scene = type.get(vehicle_name) as PackedScene;
	if !vehicle_scene:
		printerr("could not find vehicle: {v} with type {t}".format({"v": vehicle_name, "t": vehicle_type}));
		return null;
	return vehicle_scene.instantiate();
