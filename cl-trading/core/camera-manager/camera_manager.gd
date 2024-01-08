extends Camera2D

@export var move_speed: float = 100.0;
@export var zoom_speed: float = 10;
@export var max_zoom: float = 3.0;
@export var min_zoom: float = 1.0;

@onready var tile_manager: TileManager = $"../TileManager";

var min_x: float = 0.0;
var max_x: float = 0.0;
var min_y: float = 0.0;
var max_y: float = 0.0;
var zoom_vec: Vector2;

func _ready() -> void:
	var coords: Vector2 = tile_manager.map_to_local(
		Vector2i(tile_manager.map_size.x - 1, tile_manager.map_size.y - 1)
	);

	max_x = coords.x;
	max_y = coords.y
	zoom_vec = Vector2(zoom_speed, zoom_speed);

	#limit_left = min_x;
	#limit_top = min_y;
	#limit_right = max_x;
	#limit_bottom = max_y;

	position = Vector2(coords.x / 2, coords.y / 2);

func _process(delta: float) -> void:
	var new_zoom = zoom;
	if Input.is_action_just_pressed("zoom_in"):
		new_zoom += (zoom_vec * delta);
	elif Input.is_action_just_pressed("zoom_out"):
		new_zoom -= (zoom_vec * delta);

	zoom = Vector2(clampf(new_zoom.x, min_zoom, max_zoom), clampf(new_zoom.y, min_zoom, max_zoom))

	var movement_vec: Vector2 = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down");
	position += movement_vec * move_speed * delta;
