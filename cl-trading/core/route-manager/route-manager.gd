"""
RouteManager knows everything about how to create, update and delete Routes.

Say in game-play scenario where a Player uses a UI to create a Route.
In the UI, they'd chose the two cities, the appropiate vehicle, which cargo to transport etc..

When Player has entered all required information, UIMangager can simply tell RouteManager,
Hey, create me a Route for this Player with this information, and the RouteManager will:
	(1) - Instanciate needed entites
	(2) - Configure them appropiately
	(3) - Spawn Route into the world.

Same should work for edit/delete, which also happens via UI.

TODO:
	(1) - Cleanup debug-code and get out what we can of RouteManager
	(2) - Find a better way to handle/save known vehicle scenes

NOTE(cl):
	All the hard-work is being done by the actual Route node (which is C++ code), so the RouteManager
	here is just a conductor so-to-speak, and thus I'm fine with having this in GDScript.
	It tells others what to do, it actually doesn't do any expensive calculations itself.
"""
extends Node

# DEBUG SIGNALS

signal draw_route_path(route_name: String, route: PackedVector2Array);
signal clear_route_path(route_name: String);

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

# DEBUG FUNCTIONS

func handle_draw_route_signal(route_name: String, route: Array) -> void:
	emit_signal("draw_route_path", route_name, route);

func handle_clear_route_signal(route_name: String) -> void:
	emit_signal("clear_route_path", route_name);
