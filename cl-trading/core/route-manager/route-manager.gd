"""
RouteManager knows everything about how to create, update and delete Routes.
"""
class_name RouteManager extends Node

const _vehicle_scenes = [
	"res://sprites/vehicles/onshore/vehicle_horse.tscn",
	"res://sprites/vehicles/offshore/vehicle_ship.tscn"
];

var vehicles = [];

func _init() -> void:
	for scene in _vehicle_scenes:
		var loaded_scene = load(scene) as PackedScene;
		var vehicle = loaded_scene.instantiate() as TradingVehicle;
		var ctx = {
			"name": vehicle.name,
			"scene": loaded_scene,
			"tier": vehicle.tier,
			"surface": vehicle.vehicle_surface,
			"speed": vehicle.speed,
			"cargo_space": vehicle.cargo_space,
			"cost": vehicle.cost,
			"upkeep": vehicle.upkeep
		}
		vehicles.append(ctx);
		vehicle.free();

func _on_gui_create_route(ctx: Dictionary) -> void:
	var vehicle = ctx["vehicle"].instantiate() as TradingVehicle;
	if !vehicle:
		return;
	var route: Route = Route.new();
	add_child(route);
	route.set_player(ctx["player"].name);
	route.set_type(ctx["surface"]);
	route.set_kind(ctx["type"]);
	route.set_start(ctx["from"]);
	route.set_end(ctx["to"]);
	route.set_current_route(ctx["path"]);
	route.set_vehicle(vehicle);
	if !route.start():
		remove_child(route);
		route.free();
		return;
	ctx["player"].add_connection(ctx["to"]);
	ctx["player"].add_connection(ctx["from"]);
