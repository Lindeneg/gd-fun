"""
DebugManager is reponsible for connecting all debug information.
It should not know anything about other entities but it should
be able to be told about them via signals and then update accordingly.

Performance is not that critical here, as this is not production-code i.e
in a hypothetical release of the game, this code will not be shipped, so
we can afford to be a bit lazy without having too many concerns.

IMPORTANT: All code in this file runs in the editor AND the game.

If you want to control when code is run, you can make an if-statement:

if Engine.is_editor_hint():
	# code in this block runs inside editor only
else:
	# code in this block runs inside game only

Without it, code you write will run both places.

TODO:
	(1) Split into multiple files else this is going to become very big at some point.
	(2) Clearly segregate drawing functions and UI functions.
"""
@tool
extends Node2D

@export var debug_mode: bool = false:
	get:
		return debug_mode;
	set(value):
		debug_mode = value;
		if not debug_mode:
			route_debug_ui.visible = false;
		elif route_debug_ui != null:
			route_debug_ui.visible = show_route_ui;
		queue_redraw();

@export_group("City")
@export var notify_city_change: bool = false:
	get:
		return notify_city_change;
	set(value):
		city_manager.notify_tile_manager_of_cities();

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

@export_group("Routes")
@export var show_route_paths: bool = false:
	get:
		return show_route_paths;
	set(value):
		show_route_paths = value;
		queue_redraw();
@export var show_route_ui: bool = false:
	get:
		return show_route_ui;
	set(value):
		if !route_debug_ui:
			return;
		show_route_ui = value;
		route_debug_ui.visible = show_route_ui;
		queue_redraw();

													 # NONE        # GROUND    # WATER      # OBSTACLE # BRIDGE
const TILE_COLORS: Array = [Color.BLACK, Color.BLUE, Color.GREEN, Color.RED, Color.MAGENTA];
var tile_size: int = 0;
var tile_array = [];
var route_paths: Dictionary = {};

var camera_manager: Camera2D;
var tile_manager: TileManager;
var city_manager: CityManager;
var route_manager: Node;
var route_debug_ui: Control;
var route_debug_tab: HBoxContainer;

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
		if event.keycode == KEY_P:
			show_route_paths = !show_route_paths;
		if event.keycode == KEY_U:
			show_route_ui = !show_route_ui;

func _ready() -> void:
	camera_manager = get_node_or_null("../CameraManager");
	tile_manager = get_node_or_null("../TileManager");
	city_manager = get_node_or_null("../CityManager");
	route_manager = get_node_or_null("../RouteManager");
	route_debug_ui = get_node_or_null("DebugUIContainer");
	route_debug_tab = get_node_or_null("DebugUIContainer/DebugTabs/RouteDebug");

	debug_mode = false;
	route_debug_ui.visible = false;

	if !Engine.is_editor_hint():
		route_debug_tab.add_city_nodes(city_manager.get_children());

func _process(_delta: float) -> void:
	if Engine.is_editor_hint():
		return;
	if show_route_ui:
		var cam_rect = camera_manager.get_viewport_rect().size;
		route_debug_ui.position = Vector2(
			camera_manager.position.x - (cam_rect.x / camera_manager.zoom.x / 2),
			camera_manager.position.y - (cam_rect.y / camera_manager.zoom.y / 2),
		);

func _draw() -> void:
	if not debug_mode:
		return;
	if show_tiles and tile_array and tile_array.size() > 0:
		draw_tile_manager_grid();
	if show_route_paths:
		for route_key in route_paths.keys():
			var route_path: Array = route_paths[route_key];
			if route_path and route_path.size() > 0:
				draw_route_path(route_path);
	if show_route_ui:
		draw_city_names();

# SIGNAL CALLBACKS

func _on_tile_manager_draw_debug_grid(size: int, tiles: Array) -> void:
	tile_size = size;
	tile_array = tiles;
	queue_redraw();

func _on_tile_manager_remove_debug_grid() -> void:
	tile_array = [];
	tile_size = 0;
	queue_redraw();

func _on_route_manager_draw_route_path(route_name: String, route: PackedVector2Array) -> void:
	route_paths[route_name] = route;
	queue_redraw();

func _on_route_manager_clear_route_path(route_name: String) -> void:
	route_paths[route_name] = [];
	queue_redraw();

func _on_route_debug_create_debug_route(c1: String, c2: String, surface: int) -> void:
	var is_water = surface == TileManager.TILE_MAT_WATER;
	var vt = "OFFSHORE" if is_water else "ONSHORE";
	var vv = "SHIP" if is_water else "HORSE";
	var route = route_manager.create_and_init_route(c1, c2, vt, vv, surface);
	if route != null:
		route_debug_tab.created_routes.append(route);

# DRAWING FUNCTIONS

func draw_city_names():
		for child in city_manager.get_children():
			draw_string(
				ThemeDB.fallback_font,
				child.position,
				child.name,
				HORIZONTAL_ALIGNMENT_FILL,
				-1,
				14,
				Color.BLACK,
			);

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

func draw_route_path(path: Array):
	var path_size = path.size();
	for i in range(path_size):
		var current = path[i];
		if (i < path_size - 1):
			var next = path[i+1]
			var color = Color.CORNSILK if i == 0 else Color.BLACK;
			draw_line(
				tile_manager.map_to_local(current),
				tile_manager.map_to_local(next),
				color,
				2
			);
