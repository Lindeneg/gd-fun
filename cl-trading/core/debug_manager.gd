@tool
extends Node2D

@export var debug_mode: bool = false:
	get:
		return debug_mode;
	set(value):
		debug_mode = value;
		queue_redraw();

@export_category("Tile Map")

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

@export_category("Path Finding")

@export var start_path: Vector2i = Vector2i();
@export var end_path: Vector2i = Vector2i();
@export_enum("Ground:1", "Water:2") var path_mat: int = 1:
	get:
		return path_mat;
	set(value):
		path_mat = value;
		queue_redraw();
@export var show_path: bool = false:
	get:
		return show_path;
	set(value):
		show_path = value;
		queue_redraw();

const TILE_MANAGER_NAME: String = "TileManagerGrid";
const TILE_COLORS: Array = [Color.BLACK, Color.BLUE, Color.GREEN, Color.RED];
var tile_size: int = 0;
var draw_shapes: Dictionary = {};

@onready var t = $"../TileManager" as TileManager;

func _draw():
	print("draw called");
	if not debug_mode:
		return;
	for key in draw_shapes.keys():
		if key == TILE_MANAGER_NAME and show_tiles:
			draw_tile_manager_grid();
	if show_path:
		draw_path_line();

func _on_tile_manager_draw_debug_grid(size: int, tiles: Array) -> void:
	tile_size = size;
	draw_shapes[TILE_MANAGER_NAME] = tiles;
	queue_redraw();

func _on_tile_manager_remove_debug_grid() -> void:
	if draw_shapes.has(TILE_MANAGER_NAME):
		tile_size = 0;
		draw_shapes.erase(TILE_MANAGER_NAME);
		queue_redraw();

func draw_tile_manager_grid():
	var vectors = draw_shapes.get(TILE_MANAGER_NAME);
	var half_tile_size = int(tile_size / 2.0);
	for vec in vectors:
		if vec == null:
			return;
		draw_rect(
			Rect2(
				vec.coords.x - half_tile_size,
				vec.coords.y - half_tile_size,
				tile_size, tile_size
			),
			TILE_COLORS[vec.mat],
			false
		);
		if show_tile_coords:
			var local_coord = t.local_to_map(vec.coords);
			draw_tile_coord_string(Vector2(vec.coords.x - 8, vec.coords.y), str(local_coord.x));
			draw_tile_coord_string(Vector2(vec.coords.x - 8, vec.coords.y + 8), str(local_coord.y));

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

# manual testing of path finding
func draw_path_line():
	if start_path == end_path:
		return;
	var path = t.construct_path(start_path, end_path, path_mat);
	var path_size = path.size();
	for i in range(path_size):
		var current = path[i]
		if (i < path_size - 1):
			var next = path[i+1]
			var color = Color.CORNSILK if i == 0 else Color.BLACK;
			draw_line(t.map_to_local(current), t.map_to_local(next), color, 2);
