extends Control

@onready var gui: GUI = $"..";

var _from: City = null;

func _ready() -> void:
	visible = false;

func _on_city_menu_open_create_route_ui(from: City) -> void:
	_from = from;
	print("create route from ", from.name, from.get_onshore_entries(), from.get_offshore_entries());
