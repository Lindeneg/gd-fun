class_name CreateRoute extends Menu

signal create_route(ctx: Dictionary);

@onready var route_distance_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteDistanceLabel;
@onready var route_surface_opts: OptionButton = $CreateRouteRect/CreateRouteContainer/VehicleContainer/RouteSurfaceOptions;
@onready var surface_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/SurfaceValue;
@onready var speed_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/SpeedValue;
@onready var cost_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/CostValue;
@onready var upkeep_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/UpkeepValue;
@onready var space_value: Label = $CreateRouteRect/CreateRouteContainer/VehicleContainer/VehicleCtx/SpaceValue;
@onready var supply_container: GridContainer = $CreateRouteRect/CreateRouteContainer/CargoContainer/RouteResources/SupplyContainer;
@onready var demand_container: GridContainer = $CreateRouteRect/CreateRouteContainer/CargoContainer/RouteResources/DemandContainer;
@onready var route_connection: RouteConnection = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteConnection;
@onready var cargo_connection: RouteConnection = $CreateRouteRect/CreateRouteContainer/CargoContainer/CargoConnection;
@onready var current_cargo: GridContainer = $CreateRouteRect/CreateRouteContainer/CurrentCargo;
@export var gui: GUI;

var route_connection_scene = preload("res://gui/route_connection.tscn");

var _from: City = null;
var _to: Dictionary = {};
var _entryable_kind = Entryable.ENTRYABLE_CITY;
var _destinations: Array = [];
var _chosen_path: Array = [];

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

	cargo_connection.direction = cargo_connection.ArrowDirection.Right;
	cargo_connection.from_label.text = _from.name + " SUPPLY";
	cargo_connection.to_label.text = _to.name + " DEMAND";
	gui.remove_node_children(supply_container);
	gui.remove_node_children(demand_container);
	if _entryable_kind == Entryable.ENTRYABLE_CITY:
		var to_city: City = gui.city_manager.get_city(_to.name);
		for s in _from.supplies:
			gui.create_route_supply_item(s.resource_kind, supply_container, _on_route_supply_click.bind(s), true);
		for s in _from.industries:
			gui.create_route_supply_item(s.out, supply_container, _on_route_supply_click.bind(s), true);
		for s in to_city.demands:
			gui.create_demand_item(s.resource_kind, demand_container);
		for s in to_city.industries:
			gui.create_demand_item(s.in, demand_container);
	else:
		# TODO set city-resource cargo UI
		pass

	var v = selected_vehicle();
	for i in range(v.cargo_space):
		gui.create_route_supply_item(-1, current_cargo, _on_route_cargo_click.bind(i));

	gui.city_manager.lock_all_buttons();
	gui.resource_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

func stop_create() -> void:
	visible = false;
	gui.is_creating_route = false;
	_from = null;
	_to = {};
	_destinations = [];
	_chosen_path = [];
	gui.city_manager.unlock_all_buttons();
	gui.resource_manager.lock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_route_supply_click(entry):
	print("SUPPLY CLICKED: ", entry);

func _on_route_cargo_click(index):
	print("CARGO CLICKED: ", index);

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

func _on_city_menu_open_create_route_ui(from: City) -> void:
	gui.is_creating_route = true;
	_from = from;
	_destinations = gui.city_manager.get_cities_within_distance(from, gui.player.range);
	_destinations.append_array(gui.resource_manager.get_resources_within_distance(from, gui.player.range));
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
	stop_create();
	emit_signal("create_route", dict);
