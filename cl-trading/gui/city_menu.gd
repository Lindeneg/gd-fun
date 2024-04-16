class_name CityMenu extends Control

signal open_create_route_ui(from: City);

@onready var gui: GUI = $"../..";
@onready var city_label: RichTextLabel = $CityMenuRect/CityMenuContainer/CityHeader/CityLabel;
@onready var menu_supply: GridContainer = $CityMenuRect/CityMenuContainer/SupplyContainer/CitySupply;
@onready var menu_demand: GridContainer = $CityMenuRect/CityMenuContainer/DemandContainer/CityDemand;
@onready var menu_industry: GridContainer = $CityMenuRect/CityMenuContainer/IndustriesContainer/CityIndustry;

var _open: bool = false;
var _focused: bool = false;
var _city: City = null;

func _ready() -> void:
	visible = false;

func _input(event: InputEvent) -> void:
	if Engine.is_editor_hint() or !_open:
		return
	if !_focused and event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		close_menu();

func is_open() -> bool:
	return _open;

func opened_city() -> City:
	return _city;

func opened_city_name() -> StringName:
	return _city.name;

func open_menu(city: City) -> void:
	if _open:
		return;
	visible = true;
	_open = true;
	_city = city;
	city_label.text = _city.name;
	gui.city_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

	for supply in _city.supplies:
		gui.create_supply_item(supply.resource_kind, supply.amount, menu_supply);
	for demand in _city.demands:
		gui.create_demand_item(demand.resource_kind, menu_demand);
	for industry in _city.industries:
		gui.create_supply_item(industry.out, industry.amount, menu_supply);
		gui.create_demand_item(industry.in, menu_demand);
		gui.create_industry_item(industry, menu_industry);

func close_menu() -> void:
	if !_open:
		return;
	visible = false;
	_open = false;
	_city = null;
	city_label.text = "";
	gui.remove_nodes_children([menu_supply, menu_demand, menu_industry]);
	gui.city_manager.unlock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_build_route_btn_button_down() -> void:
	if !_open:
		return;
	emit_signal("open_create_route_ui", _city);
	close_menu();

func _on_mouse_entered() -> void:
	_focused = true;

func _on_mouse_exited() -> void:
	_focused = false;
