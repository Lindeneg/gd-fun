"""
CameraManger is reponsible for handling camera
updates via user inputs, such as movement and zooming.

Right now it reactes on arrow-keys and mouse-scroll.

TODO:
	(1) Support mouse-drag for movement.
	(2) Play with smoothing settings to make the "feel" right.
	(3) Perhaps change Anchor Mode to "Drag Center" instead of "Fixed TopLeft".

(1) NOTE: this seem to be configureable via the numerous "Drag" options on camera itself.
(3) NOTE: this will affect the current limit logic inside "handle_camera_movement".
"""
extends Camera2D

@export var move_speed: float = 100.0;
@export var zoom_speed: int = 10;
@export var max_zoom: float = 1;
@export var min_zoom: float = 1;

@onready var tile_manager: TileManager = $"../TileManager";

const ZOOM_VEC = Vector2(1, 1);

func _ready() -> void:
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

func _process(delta: float) -> void:
	handle_camera_movement(delta);
	#handle_zoom_input(delta);

func handle_camera_movement(delta: float) -> void:
	var movement_vec: Vector2 = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down");
	# Save what-would-be the new position in a temporary variable
	# we need to check that it is not out-of-bounds before we use
	# it to update the actual camera position itself.
	var new_pos = position + (movement_vec * move_speed * delta);

	var viewport_rect = get_viewport_rect();
	var half_x = (viewport_rect.size.x / zoom.x) / 2;
	var half_y = (viewport_rect.size.y / zoom.y) / 2;
	if new_pos.x - half_x < 0:
		new_pos.x = half_x;
	elif new_pos.x + half_x > limit_right:
		new_pos.x = limit_right - half_x;
	if new_pos.y - half_y < 0:
		new_pos.y = half_y;
	elif new_pos.y + half_y > limit_bottom:
		new_pos.y = limit_bottom - half_y;

	position = new_pos;

func handle_zoom_input(delta: float) -> void:
	if Input.is_action_just_pressed("zoom_in"):
		handle_zoom_action(delta);
	elif Input.is_action_just_pressed("zoom_out"):
		handle_zoom_action(delta, -1);

func handle_zoom_action(delta: float, multiplier = 1) -> void:
	var new_zoom = zoom;
	new_zoom += (ZOOM_VEC * zoom_speed * delta) * multiplier;
	# Never go below min_zoom or above max_zoom.
	zoom = Vector2(
		clampf(new_zoom.x, min_zoom, max_zoom),
		clampf(new_zoom.y, min_zoom, max_zoom)
	);
