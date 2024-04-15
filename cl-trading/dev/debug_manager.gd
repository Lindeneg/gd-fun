"""
DebugManager is reponsible for connecting all debug information.

Performance is not that critical here, as this is not production-code i.e
in a hypothetical release of the game, this code will not be shipped, so
we can afford to be a bit lazy without having too many concerns.
"""
@tool
extends Node2D

@export var debug_mode: bool = false:
	get:
		return debug_mode;
	set(value):
		debug_mode = value;
		queue_redraw();

@export_group("Placeable")
@export var notify_city_change: bool = false:
	get:
		return notify_city_change;
	set(value):
		city_manager.notify_tile_manager();

@export var notify_resource_change: bool = false:
	get:
		return notify_resource_change;
	set(value):
		resource_manager.notify_tile_manager();

@export_group("Tile Map")
@export var show_tiles: bool = false:
	get:
		return show_tiles;
	set(value):
		show_tiles = value;
		queue_redraw();
@export var show_tile_coords: bool = false:
	get:
		return show_tile_coords;
	set(value):
		show_tile_coords = value;
		if (show_tile_coords and !show_tiles):
			show_tiles = true;
			return;
		queue_redraw();
@export var rebuild_debug_graph: bool = false:
	get:
		return rebuild_debug_graph;
	set(value):
		tile_manager.set_rebuild_debug_graph(true);
													 # NONE        # GROUND    # WATER      # OBSTACLE # BRIDGE
const TILE_COLORS: Array = [Color.BLACK, Color.BLUE, Color.GREEN, Color.RED, Color.MAGENTA];
var tile_size: int = 0;
var tile_array = [];

var camera_manager: Camera2D;
var tile_manager: TileManager;
var city_manager: CityManager;
var resource_manager: ResourceManager;
var route_manager: Node;

# NOTIFICATION CALLBACKS

func _unhandled_input(event):
	if Engine.is_editor_hint():
		return;
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			get_tree().quit();
		if event.keycode == KEY_D:
			debug_mode = !debug_mode;
		if event.keycode == KEY_T:
			show_tiles = !show_tiles;
		if event.keycode == KEY_C:
			show_tile_coords = !show_tile_coords;

func _ready() -> void:
	camera_manager = get_node_or_null("../CameraManager");
	tile_manager = get_node_or_null("../TileManager");
	city_manager = get_node_or_null("../CityManager");
	route_manager = get_node_or_null("../RouteManager");
	resource_manager = get_node_or_null("../ResourceManager");

	debug_mode = false;

func _draw() -> void:
	if not debug_mode:
		return;
	if show_tiles and tile_array and tile_array.size() > 0:
		draw_tile_manager_grid();

# SIGNAL CALLBACKS

func _on_tile_manager_draw_debug_grid(size: int, tiles: Array) -> void:
	tile_size = size;
	tile_array = tiles;
	queue_redraw();

func _on_tile_manager_remove_debug_grid() -> void:
	tile_array = [];
	tile_size = 0;
	queue_redraw();

# DRAWING FUNCTIONS

func draw_tile_manager_grid():
	var half_tile_size = int(tile_size / 2.0);
	for tile in tile_array:
		if tile == null:
			return;
		draw_rect(
			Rect2(
				tile.coords.x - half_tile_size,
				tile.coords.y - half_tile_size,
				tile_size, tile_size
			),
			TILE_COLORS[tile.surface],
			false
		);
		if show_tile_coords:
			var local_coord = tile_manager.local_to_map(tile.coords);
			draw_tile_coord_string(
				Vector2(tile.coords.x - half_tile_size, tile.coords.y),
				str(local_coord.x)
			);
			draw_tile_coord_string(
				Vector2(tile.coords.x - half_tile_size, tile.coords.y + half_tile_size),
				str(local_coord.y)
			);

func draw_tile_coord_string(vec: Vector2, s: String):
		draw_string(
			ThemeDB.fallback_font,
			vec,
			s,
			HORIZONTAL_ALIGNMENT_FILL,
			-1,
			8,
			Color.BLACK,
		);
