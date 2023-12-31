@tool
extends Node2D

@export var debug_mode: bool = false;

const TILE_MANAGER_NAME: String = "TileManager";
var tile_size: int = 0;
var draw_shapes: Dictionary = {};

func _process(_delta: float) -> void:
	queue_redraw();

func _draw():
	if not debug_mode:
		return;
	for key in draw_shapes.keys():
		if key == TILE_MANAGER_NAME:
			draw_tile_manager();

func _on_tile_manager_draw_debug_grid(size: int, tiles: Array) -> void:
	tile_size = size;
	draw_shapes[TILE_MANAGER_NAME] = tiles;

func _on_tile_manager_remove_debug_grid() -> void:
	if draw_shapes.has(TILE_MANAGER_NAME):
		tile_size = 0;
		draw_shapes.erase(TILE_MANAGER_NAME);

func draw_tile_manager():
	var vectors = draw_shapes.get(TILE_MANAGER_NAME);
	var half_tile_size = int(tile_size / 2.0);
	for vec in vectors:
		if vec != null:
			draw_rect(
				Rect2(
					vec.coords.x - half_tile_size,
					vec.coords.y - half_tile_size,
					tile_size, tile_size
				),
				vec.color,
				false # do not fill
			);
