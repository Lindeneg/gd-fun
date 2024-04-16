extends Control

@onready var gui: GUI = $"../..";

var _from: City = null;
var _to: Entryable = null;

func _ready() -> void:
	visible = false;

func _on_city_menu_open_create_route_ui(from: City) -> void:
	_from = from;
	gui.city_manager.get_cities_within_distance(from, 100);
	#print("create route from ", from.name, from.get_onshore_entries(), from.get_offshore_entries());
