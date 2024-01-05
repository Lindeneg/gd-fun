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
		tile_route = tile_manager.construct_path(start, end, route_surface);
		emit_signal("draw_debug_path", name, tile_route if show_debug_path else []);

@onready var vehicle = $"Ship" as TradingVehicle;
@onready var tile_manager = $"../TileManager" as TileManager;

var tile_route = [];

func _ready() -> void:
	tile_route = tile_manager.construct_path(start, end, route_surface);
	var map_route = [];
	for coord in tile_route:
		map_route.append(tile_manager.map_to_local(coord));
	vehicle.set_map_path(map_route);
	vehicle.start_navigating();

func _on_ship_destination_reached(_destination: Vector2) -> void:
	pass
	#vehicle.start_navigating();
