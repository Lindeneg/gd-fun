extends Node

@onready var tile_manager = $"../TileManager" as TileManager;

const DEFAULT_COLLISION_YIELDER = Vector2(-1, 0);

var handling_dict: Dictionary = {};

func collision_callback(v1: TradingVehicle, v2: TradingVehicle):
	if v1.vehicle_surface != v2.vehicle_surface:
		return;
	var r1 = v1.get_rid().get_id();
	var r2 = v2.get_rid().get_id();
	if handling_dict.get(r1) == r2 and handling_dict.get(r2) == r1:
		return;
	handling_dict[r1] = r2;
	handling_dict[r2] = r1;
	var prio = resolve_priority(v1, v2);
	var prio_dir = prio[0].get_direction();
	var coords = tile_manager.local_to_map(tile_manager.to_local(prio[1].position));
	var free_tile = tile_manager.find_free_tile_on_surface(coords, prio[1].vehicle_surface, prio_dir);
	if free_tile == coords:
		return;
	prio[1].set_navigation_target(tile_manager.map_to_local(free_tile));

func resolve_priority(v1: TradingVehicle, v2: TradingVehicle) -> Array:
	if v1.vehicle_priority > v2.vehicle_priority:
		return [v1, v2];
	elif v1.vehicle_priority < v2.vehicle_priority:
		return [v2, v1];
	var v1_direction = v1.get_direction();
	if v1_direction == DEFAULT_COLLISION_YIELDER:
		return [v2, v1];
	return [v1, v2];
