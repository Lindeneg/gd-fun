@tool
extends Route

signal draw_debug_path(name: String, v: Array);

@export_category("Path")
@export var start: Vector2i = Vector2i.ZERO;
@export var end: Vector2i = Vector2i.ZERO;
@export_enum("Ground:1", "Water:2") var route_surface: int = 1;
@export var wait_time_sec: int = 5;

@export_category("Debug")
@export var show_debug_path: bool = false:
	get:
		return show_debug_path;
	set(value):
		show_debug_path = value;
		emit_signal("draw_debug_path", name, tile_route if show_debug_path else []);

@onready var vehicle = $"Ship" as TradingVehicle;
var tile_manager: TileManager;

var tile_route = [];

func _on_tile_manager_ready() -> void:
	tile_manager = $"../TileManager" as TileManager;
	if Engine.is_editor_hint():
		tile_route = tile_manager.construct_path(start, end, route_surface);
		emit_signal("draw_debug_path", name, tile_route if show_debug_path else []);

func _ready() -> void:
	if Engine.is_editor_hint():
		return;
	tile_route = tile_manager.construct_path(start, end, route_surface);
	if show_debug_path:
		emit_signal("draw_debug_path", name, tile_route);
	var map_route = [];
	for coord in tile_route:
		map_route.append(tile_manager.map_to_local(coord));
	vehicle.set_map_path(map_route);
	vehicle.start_navigating();

func _on_ship_destination_reached(_destination: Vector2) -> void:
	if Engine.is_editor_hint():
		return;
	#vehicle.start_navigating();

