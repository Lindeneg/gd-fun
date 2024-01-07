extends Node

# DEBUG SIGNALS

signal draw_route_path(route_name: String, route: PackedVector2Array);
signal clear_route_path(route_name: String);

# CONSTANTS

# Dictionaries with Vehicle information
# IMPORTANT:
# add here when you create new vehicle scenes
const VEHICLES = {
	"ONSHORE": {
		"HORSE": preload("res://dev/trading_horse.tscn"),
	},
	"OFFSHORE": {
		"SHIP": preload("res://sprites/ship.tscn")
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
	var route = Route.new();
	var vehicle = instanciate_vehicle(vehicle_type, vehicle_name);
	if !vehicle:
		printerr("failed to create route {c1}<->{c2} with vehicle {v} - {t}".format(
			{"c1": city1, "c2": city2, "v": vehicle_name, "t": vehicle_type}
		));
		return;

	# TEMP
	route.connect("draw_debug_route", handle_draw_route_signal);
	route.connect("remove_debug_route", handle_clear_route_signal);

	add_child(route);

	route.set_type(surface);
	route.set_city_one(city1);
	route.set_city_two(city2);
	route.set_vehicle(vehicle);

	# TEMP
	route.set_debug_mode(true);

	route.start();

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
	#if vehicle_scene.can_instantiate():
	#	printerr("cannot instanciate vehicle: {v} with type {t}".format({"v": vehicle_name, "t": vehicle_type}));
	#	return null;
	return vehicle_scene.instantiate();

# DEBUG FUNCTIONS

func handle_draw_route_signal(route_name: String, route: Array) -> void:
	emit_signal("draw_route_path", route_name, route);

func handle_clear_route_signal(route_name: String) -> void:
	emit_signal("clear_route_path", route_name);
