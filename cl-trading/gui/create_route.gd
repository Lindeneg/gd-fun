class_name CreateRoute extends Control

@onready var gui: GUI = $"../..";

var _from: City = null;
var _to: Entryable = null;
var _destinations: Array = [];

func _ready() -> void:
	visible = false;

func _unhandled_input(event):
	if gui.is_creating_route and event is InputEventKey and event.pressed:
		if event.keycode == KEY_Q:
			stop_create();

func set_to(to: Entryable) -> void:
	_to = to;
	print("FROM ", _from.name, " TO ", _to.name);
	visible = true;
	gui.city_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

func stop_create() -> void:
	gui.is_creating_route = false;
	_from = null;
	_destinations = [];
	gui.city_manager.unlock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_city_menu_open_create_route_ui(from: City) -> void:
	gui.is_creating_route = true;
	_from = from;
	_destinations = gui.city_manager.get_cities_within_distance(from, 100);
	var names = [];
	for dest in _destinations:
		names.push_back(dest.name);
	gui.city_manager.lock_buttons_except(names);
	gui.camera_manager.unlock_cam();
