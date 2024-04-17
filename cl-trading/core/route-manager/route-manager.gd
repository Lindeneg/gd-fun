"""
RouteManager knows everything about how to create, update and delete Routes.
"""
class_name RouteManager extends Node

# CONSTANTS

# Dictionaries with Vehicle information
# NOTE(cl): we can probably find a better way to do it
const _vehicles = {
	"vehicle_horse": {
		"scene": preload("res://sprites/vehicles/onshore/vehicle_horse.tscn"),
		"surface": Utils.TILE_SURFACE_GROUND
	},
	"vehicle_ship": {
		"scene": preload("res://sprites/vehicles/offshore/vehicle_ship.tscn"),
		"surface": Utils.TILE_SURFACE_WATER
	}
};

# CORE FUNCTIONS

func instanciate_vehicle(vehicle_scene: PackedScene) -> TradingVehicle:
	if !vehicle_scene.can_instantiate():
		return null;
	return vehicle_scene.instantiate();

func _on_gui_create_route(ctx: Dictionary) -> void:
	var vehicle = instanciate_vehicle(ctx["vehicle"]);
	if !vehicle:
		return;
	var route: Route = Route.new();
	add_child(route);

	route.set_type(ctx["surface"]);
	route.set_kind(ctx["type"]);
	route.set_start(ctx["from"]);
	route.set_end(ctx["to"]);
	route.set_current_route(ctx["path"]);
	route.set_vehicle(vehicle);

	var did_start = route.start();
	if !did_start:
		remove_child(route);
		route.free();
