class_name CreateRoute extends Menu

signal create_route(ctx: Dictionary);

@onready var route_distance_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteDistanceLabel;
@onready var route_surface_opts: OptionButton = $CreateRouteRect/CreateRouteContainer/VehicleContainer/RouteSurfaceOptions;
@onready var surface_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/SurfaceValue;
@onready var speed_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/SpeedValue;
@onready var cost_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/CostValue;
@onready var upkeep_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/UpkeepValue;
@onready var space_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/SpaceValue;
@onready var cargo_container: VBoxContainer = $CreateRouteRect/CreateRouteContainer/CargoContainer;
@onready var supply_container: GridContainer = $CreateRouteRect/CreateRouteContainer/CargoContainer/SelectionContainer/SupplySelection/SupplyContainer;
@onready var demand_container: GridContainer = $CreateRouteRect/CreateRouteContainer/CargoContainer/SelectionContainer/DemandSelection/DemandContainer;
@onready var route_connection: RouteConnection = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteConnection;
@onready var chosen_cargo_container: GridContainer = $CreateRouteRect/CreateRouteContainer/CurrentCargo;
@onready var cargo_title: Label = $CreateRouteRect/CreateRouteContainer/CargoTitle;
@onready var from_label: Label = $CreateRouteRect/CreateRouteContainer/CargoContainer/SelectionContainer/SupplySelection/Label;
@onready var to_label: Label = $CreateRouteRect/CreateRouteContainer/CargoContainer/SelectionContainer/DemandSelection/Label;
@onready var confirm_btn: Button = $CreateRouteRect/CreateRouteContainer/RouteActions/RouteConfirmBtn;

@export var gui: GUI;

var _from: City = null;
var _to: Dictionary = {};
var _direction = TradingVehicle.VEHICLE_MOVE_DIR_AB;
var _entryable_kind = Entryable.ENTRYABLE_CITY;
var _destinations: Array = [];
var _chosen_path: Array = [];
var _chosen_cargo: Dictionary = {
	TradingVehicle.VEHICLE_MOVE_DIR_AB: {
		"total_space": 0,
		"used_space": 0,
		"cargo": []
	},
	TradingVehicle.VEHICLE_MOVE_DIR_BA: {
		"total_space": 0,
		"used_space": 0,
		"cargo": []
	},
};
var _player_gold = 0;
var _vehicle_cost = 0;

var vehicles = [];

func _ready() -> void:
	vehicles = gui.route_manager.vehicles;
	for i in range(vehicles.size()):
		route_surface_opts.add_item(vehicles[i].name, i);

func selected_vehicle():
	return vehicles[route_surface_opts.get_item_id(route_surface_opts.selected)];

func set_to(to: Entryable) -> void:
	var destination = null;
	for dest in _destinations:
		if dest["name"] == to.name:
			destination = dest;
			break;
	assert(destination != null, "destination is null");
	_entryable_kind = to.get_kind();
	_to = destination;
	open_menu();

func open_menu() -> void:
	if visible:
		return;
	visible = true;

	route_connection.from_label.text = _from.name;
	route_connection.to_label.text = _to.name;
	_direction = TradingVehicle.VEHICLE_MOVE_DIR_AB;

	preselect_vehicle();
	if _entryable_kind == Entryable.ENTRYABLE_CITY:
		cargo_container.visible = true;
		cargo_title.visible = true;
		chosen_cargo_container.visible = true;
		update_city_city_cargo_view();
	else:
		cargo_container.visible = false;
		cargo_title.visible = false;
		chosen_cargo_container.visible = false;

	player_gold_changed(gui.player_finance.get_gold_());

	gui.city_manager.lock_all_buttons();
	gui.resource_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

func preselect_vehicle() -> void:
	var has_onshore = _to["onshores"].size() > 0;
	var has_offshore = _to["offshores"].size() > 0;
	var onshore_idx = -1;
	var offshore_idx = -1;
	for i in range(vehicles.size()):
		var vehicle = vehicles[i];
		var item_idx = route_surface_opts.get_item_index(i);
		if (vehicle.surface == Utils.TILE_SURFACE_GROUND and !has_onshore) or (vehicle.surface == Utils.TILE_SURFACE_WATER and !has_offshore):
			route_surface_opts.set_item_disabled(item_idx, true);
			continue;
		route_surface_opts.set_item_disabled(item_idx, false);
		if onshore_idx == -1 and vehicle.surface == Utils.TILE_SURFACE_GROUND:
			onshore_idx = item_idx;
		if offshore_idx == -1 and vehicle.surface == Utils.TILE_SURFACE_WATER:
			offshore_idx = item_idx;
	if onshore_idx > -1:
		route_surface_opts.select(onshore_idx);
		_set_path_from_idx(onshore_idx);
	elif offshore_idx > -1:
		route_surface_opts.select(offshore_idx);
		_set_path_from_idx(offshore_idx);

func player_gold_changed(new_amount: int) -> void:
	_player_gold = new_amount;
	update_buy_button_state();

func get_free_cargo_space() -> int:
	var cargo_ctx = _chosen_cargo[_direction];
	return cargo_ctx["total_space"] - cargo_ctx["used_space"];

func update_cargo_title() -> void:
	cargo_title.text = "CURRENT CARGO {used}/{limit}".format({
		"used": _chosen_cargo[_direction]["used_space"],
		"limit": _chosen_cargo[_direction]["total_space"]
	});

func update_current_cargo_view() -> void:
	gui.remove_node_children(chosen_cargo_container);
	var cargo_ctx = _chosen_cargo[_direction];
	var used = 0;
	var i = 0;
	while used < cargo_ctx["total_space"]:
		var cargo_resource = -1;
		if cargo_ctx["cargo"].size() > i:
			cargo_resource = cargo_ctx["cargo"][i];
		used += gui.create_route_supply_item(cargo_resource, chosen_cargo_container, _on_route_cargo_click.bind(i), Color.ALICE_BLUE);
		i += 1

func update_buy_button_state() -> void:
	if _player_gold < _vehicle_cost:
		confirm_btn.disabled = true;
		confirm_btn.mouse_default_cursor_shape = Control.CURSOR_FORBIDDEN;
		confirm_btn.tooltip_text = "Not enough gold";
	else:
		confirm_btn.disabled = false;
		confirm_btn.mouse_default_cursor_shape = Control.CURSOR_POINTING_HAND;
		confirm_btn.tooltip_text = "";

func update_city_city_cargo_view() -> void:
	gui.remove_node_children(supply_container);
	gui.remove_node_children(demand_container);

	update_current_cargo_view();

	var a = _from;
	var b = _to;
	if _direction == TradingVehicle.VEHICLE_MOVE_DIR_BA:
		a = _to;
		b = _from;

	from_label.text = a.name + " (supply)";
	to_label.text = b.name + " (demand)";
	var from_city: City = a if is_instance_of(a, City) else gui.city_manager.get_city(a.name);
	var to_city: City = b if is_instance_of(b, City) else gui.city_manager.get_city(b.name);
	for s in from_city.supplies:
		gui.create_route_supply_item(s.resource_kind, supply_container, _on_route_supply_click.bind(s.resource_kind));
	for s in from_city.industries:
		gui.create_route_supply_item(s.out, supply_container, _on_route_supply_click.bind(s.out));
	for s in to_city.demands:
		gui.create_demand_item(s.resource_kind, demand_container);
	for s in to_city.industries:
		gui.create_demand_item(s.in, demand_container);

func stop_create() -> void:
	visible = false;
	gui.is_creating_route = false;
	_from = null;
	_to = {};
	_destinations = [];
	_chosen_path = [];
	_player_gold = 0;
	_vehicle_cost = 0;
	gui.remove_node_children(supply_container);
	gui.remove_node_children(demand_container);
	gui.remove_node_children(chosen_cargo_container);
	gui.city_manager.unlock_all_buttons();
	gui.resource_manager.lock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_route_supply_click(entry):
	var res = gui.base_resource_manager.get_resource(entry);
	if get_free_cargo_space() - res.weight >= 0:
		_chosen_cargo[_direction]["cargo"].append(entry);
		_chosen_cargo[_direction]["used_space"] += res.weight;
		update_cargo_title();
		update_current_cargo_view();

func _on_route_cargo_click(index):
	if (index >= _chosen_cargo[_direction]["cargo"].size()):
		return;
	var res = gui.base_resource_manager.get_resource(_chosen_cargo[_direction]["cargo"][index]);
	_chosen_cargo[_direction]["cargo"].remove_at(index);
	_chosen_cargo[_direction]["used_space"] = max(0, _chosen_cargo[_direction]["used_space"] - res.weight);
	update_cargo_title();
	update_current_cargo_view();

func _set_path_from_idx(idx: int) -> void:
	return _set_path_from_id(route_surface_opts.get_item_id(idx));

func _set_path_from_id(vehicle_id: int) -> void:
	var vehicle = vehicles[vehicle_id];
	var is_ground = vehicle.surface == Utils.TILE_SURFACE_GROUND;
	_chosen_path = _to["onshores"] if is_ground else _to["offshores"]
	route_distance_label.text = "Distance: %d" % _chosen_path.size();
	surface_value.text = "Ground" if is_ground else "Water";
	speed_value.text = "%d" % vehicle.speed;
	cost_value.text = "%d" % vehicle.cost;
	upkeep_value.text = "%d" % vehicle.upkeep;
	space_value.text = "%d" % vehicle.cargo_space;

	_vehicle_cost = vehicle.cost;

	var total_space = vehicle.cargo_space;
	_chosen_cargo[TradingVehicle.VEHICLE_MOVE_DIR_AB]["total_space"] = total_space;
	_chosen_cargo[TradingVehicle.VEHICLE_MOVE_DIR_AB]["used_space"] = 0;
	_chosen_cargo[TradingVehicle.VEHICLE_MOVE_DIR_AB]["cargo"] = [];
	_chosen_cargo[TradingVehicle.VEHICLE_MOVE_DIR_BA]["total_space"] = total_space;
	_chosen_cargo[TradingVehicle.VEHICLE_MOVE_DIR_BA]["used_space"] = 0;
	_chosen_cargo[TradingVehicle.VEHICLE_MOVE_DIR_BA]["cargo"] = [];
	update_buy_button_state();
	update_cargo_title();
	update_current_cargo_view();

func _on_city_menu_open_create_route_ui(from: City) -> void:
	gui.is_creating_route = true;
	_from = from;
	_destinations = gui.city_manager.get_cities_within_distance(from, gui.player.get_range());
	_destinations.append_array(gui.resource_manager.get_resources_within_distance(from, gui.player.get_range()));
	var names = [];
	for dest in _destinations:
		names.push_back(dest.name);
	gui.city_manager.lock_buttons_except(names);
	gui.resource_manager.lock_buttons_except(names);
	gui.camera_manager.unlock_cam();

func _on_close_menu() -> void:
	stop_create();

func _on_route_surface_options_item_selected(index: int) -> void:
	_set_path_from_idx(index);

func _on_route_cancel_btn_button_down() -> void:
	stop_create();

func _on_route_confirm_btn_button_down() -> void:
	var dict = {};
	var vehicle = selected_vehicle();
	dict["player"] = gui.player;
	dict["from"] = _from.name;
	dict["to"] = _to.name;
	dict["path"] = _chosen_path;
	dict["type"] = _entryable_kind;
	dict["surface"] = vehicle.surface;
	dict["vehicle"] = vehicle.scene;
	dict["cargo"] = _chosen_cargo;
	if _entryable_kind == Entryable.ENTRYABLE_RESOURCE:
		var res = gui.resource_manager.get_resource(_to.name);
		dict["res"] = res.resource_kind;
	stop_create();
	emit_signal("create_route", dict);

func _on_switch_btn_button_down() -> void:
	if _direction == TradingVehicle.VEHICLE_MOVE_DIR_AB:
		_direction = TradingVehicle.VEHICLE_MOVE_DIR_BA;
	else:
		_direction = TradingVehicle.VEHICLE_MOVE_DIR_AB;
	update_city_city_cargo_view();
	update_cargo_title();
	update_current_cargo_view();
