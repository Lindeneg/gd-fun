@tool
extends Node2D

@export var debug_mode: bool = false:
	get:
		return debug_mode;
	set(value):
		debug_mode = value;
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

													 # NONE        # GROUND    # WATER      # OBSTACLE
const TILE_COLORS: Array = [Color.BLACK, Color.BLUE, Color.GREEN, Color.RED];
var tile_size: int = 0;
var tile_array = [];
var route_paths: Dictionary = {};

@onready var tile_manager = $"../TileManager" as TileManager;
@onready var city_manager = $"../CityManager" as CityManager;

func _draw():
	if not debug_mode:
		return;
	if show_tiles and tile_array and tile_array.size() > 0:
		draw_tile_manager_grid();
	if show_route_paths:
		for route_key in route_paths.keys():
			var route_path: Array = route_paths[route_key];
			if route_path and route_path.size() > 0:
				draw_route_path(route_path);

func _on_tile_manager_draw_debug_grid(size: int, tiles: Array) -> void:
	tile_size = size;
	tile_array = tiles;
	queue_redraw();

func _on_tile_manager_remove_debug_grid() -> void:
	tile_array = [];
	tile_size = 0;
	queue_redraw();



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
			TILE_COLORS[tile.mat],
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

func _on_route_a_draw_debug_path(route_name: String, v: Array) -> void:
	route_paths[route_name] = v;
	queue_redraw();
