"""
CameraManger is reponsible for handling camera
updates via mouse-movement and displaying related UI.
"""
class_name CameraManager extends Camera2D

signal cam_speed_changed(new_speed: float);

@export var base_move_speed: float = 1.5;
@export var change_step: float = 0.5;
@export var max_move_speed: float = 10;
@export var min_move_speed: float = 0.0;
@export var move_cutoff: float = 200;

@onready var tile_manager: TileManager = $"../TileManager";
@onready var gui: Control = $GUI;

var cam_locked: bool = false;

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_CONFINED;
	# We need to know the max x and y of our map.
	# TileManager can tell us about it but we get
	# the result back in tile-space coords i.e
	# the coords you see when we draw the debug tile-map.

	# We need to convert them from tile-space to local-space i.e
	# the position value you see in the Transform component.

	# This is what tile_manager.map_to_local() does for us.
	# tile_manager.local_to_map() goes the other way.
	var max_coords: Vector2 = tile_manager.map_to_local(
		Vector2i(tile_manager.map_size.x, tile_manager.map_size.y)
	);

	# Using half a tile size as an offset, this can be tweaked
	var half_tile_size = tile_manager.tile_set.tile_size.x / 2.0;

	# This is the actual limit property on the Camera2D itself.
	# Now we have the information we need to set it correctly.
	limit_left = int(0);
	limit_top = int(0);
	limit_right = int(max_coords.x - half_tile_size);
	limit_bottom = int(max_coords.y - half_tile_size);

	# Adjust main GUI container size accouting for current zoom.
	var viewport_rect = get_viewport_rect();
	gui.custom_minimum_size.x = viewport_rect.size.x / zoom.x;
	gui.custom_minimum_size.y = viewport_rect.size.y / zoom.y;

func _process(delta: float) -> void:
	if cam_locked:
		return;
	handle_change_speed();
	handle_camera_movement(delta);

func get_half_xy() -> Dictionary:
	var viewport_rect = get_viewport_rect();
	return {
		"x": (viewport_rect.size.x / zoom.x) / 2,
		"y": (viewport_rect.size.y / zoom.y) / 2
	};

func lock_cam() -> void:
	cam_locked = true;

func unlock_cam() -> void:
	cam_locked = false;

func handle_camera_movement(delta: float) -> void:
	var diff = get_global_mouse_position() - get_screen_center_position();
	var movement_vec = Vector2.ZERO;
	if abs(diff.x) > move_cutoff:
		movement_vec.x = diff.x;
	if abs(diff.y) > move_cutoff:
		movement_vec.y = diff.y;
	if movement_vec.x == 0 and movement_vec.y == 0:
		return;
	# Save what-would-be the new position in a temporary variable
	# we need to check that it is not out-of-bounds before we use
	# it to update the actual camera position itself.
	var new_pos = position + (movement_vec * base_move_speed * delta);
	var half = get_half_xy();
	if new_pos.x - half.x < 0:
		new_pos.x = half.x;
	elif new_pos.x + half.x > limit_right:
		new_pos.x = limit_right - half.x;
	if new_pos.y - half.y < 0:
		new_pos.y = half.y;
	elif new_pos.y + half.y > limit_bottom:
		new_pos.y = limit_bottom - half.y;

	position = new_pos;

func handle_change_speed() -> void:
	var multiplier = 0;
	if Input.is_action_just_pressed("zoom_in"):
		multiplier = 1;
	elif Input.is_action_just_pressed("zoom_out"):
		multiplier = -1;
	if multiplier == 0:
		return;
	var new_speed = clampf(base_move_speed + (change_step * multiplier), min_move_speed, max_move_speed);
	if base_move_speed == new_speed:
		return;
	base_move_speed = new_speed;
	cam_speed_changed.emit(base_move_speed);
