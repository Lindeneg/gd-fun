class_name CityMenu extends Menu

signal open_create_route_ui(from: City);

@onready var gui: GUI = $"../..";
@onready var city_label: RichTextLabel = $CityMenuRect/CityMenuContainer/CityHeader/CityLabel;
@onready var menu_supply: GridContainer = $CityMenuRect/CityMenuContainer/SupplyContainer/CitySupply;
@onready var menu_demand: GridContainer = $CityMenuRect/CityMenuContainer/DemandContainer/CityDemand;
@onready var menu_industry: GridContainer = $CityMenuRect/CityMenuContainer/IndustriesContainer/CityIndustry;
@onready var route_btn: Button = $CityMenuRect/CityMenuContainer/CityHeader/BuildButtons/BuildRouteBtn;
@onready var industry_btn: Button = $CityMenuRect/CityMenuContainer/CityHeader/BuildButtons/BuildIndustryBtn;

var _city: City = null;

func _update_btn_state(city_name: StringName) -> void:
	if gui.player.get_connections().has(city_name):
		route_btn.disabled = false;
		industry_btn.disabled = false;
		route_btn.mouse_default_cursor_shape = Control.CURSOR_POINTING_HAND;
		industry_btn.mouse_default_cursor_shape = Control.CURSOR_POINTING_HAND;
	else:
		route_btn.disabled = true;
		industry_btn.disabled = true;
		route_btn.mouse_default_cursor_shape = Control.CURSOR_FORBIDDEN;
		industry_btn.mouse_default_cursor_shape = Control.CURSOR_FORBIDDEN;

func open_menu(city: City) -> void:
	if visible:
		return;
	if gui.is_creating_route:
		gui.set_create_route_to(city);
		return;
	visible = true;
	_city = city;
	city_label.text = _city.name;
	gui.city_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

	_update_btn_state(city.name);

	for supply in _city.supplies:
		gui.create_supply_item(supply.resource_kind, supply.amount, menu_supply);
	for demand in _city.demands:
		gui.create_demand_item(demand.resource_kind, menu_demand);
	for industry in _city.industries:
		gui.create_supply_item(industry.out, industry.amount, menu_supply);
		gui.create_demand_item(industry.in, menu_demand);
		gui.create_industry_item(industry, menu_industry);

func close_menu_unlocked() -> void:
	if !visible:
		return;
	close_menu_locked();
	gui.city_manager.unlock_all_buttons();
	gui.camera_manager.unlock_cam();

func close_menu_locked() -> void:
	if !visible:
		return;
	visible = false;
	_city = null;
	city_label.text = "";
	gui.remove_nodes_children([menu_supply, menu_demand, menu_industry]);

func _on_build_route_btn_button_down() -> void:
	if !visible:
		return;
	var tmp = _city;
	close_menu_locked();
	emit_signal("open_create_route_ui", tmp);

func _on_close_menu() -> void:
	close_menu_unlocked();
