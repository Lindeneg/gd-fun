@tool
extends Route

signal draw_debug_path(name: String, v: Array);

@export_category("Path")
@export var start: City;
@export var end: City;
@export_enum("Ground:1", "Water:2") var route_surface: int = 1;

@export_category("Debug")
@export var show_debug_path: bool = false:
	get:
		return show_debug_path;
	set(value):
		show_debug_path = value;
		emit_signal("draw_debug_path", name, tile_route if show_debug_path else []);

@onready var timer = $RouteATimer as Timer;
@onready var vehicle = $"Ship" as TradingVehicle;

@onready var tile_manager = $"../../TileManager" as TileManager;

var tile_route = [];
var reached_at = null;

func _ready() -> void:
	if Engine.is_editor_hint():
		return;
	if !start || !end:
		return;

	var start_entries = start.get_offshore_entries();
	var end_entries = end.get_offshore_entries();
	if start_entries.size() == 0 || end_entries.size() == 0:
		return;
	tile_route = tile_manager.construct_path(start_entries[0], end_entries[0], route_surface);
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
	timer.start();

func _on_route_a_timer_timeout() -> void:
	vehicle.start_navigating();
