class_name CreateRoute extends Menu

signal create_route(ctx: Dictionary);

@onready var from_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteConnection/FromLabel;
@onready var to_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteConnection/ToLabel;
@onready var route_distance_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteDistanceLabel;
@onready var route_surface_opts: OptionButton = $CreateRouteRect/CreateRouteContainer/VehicleContainer/RouteSurfaceOptions;

@export var gui: GUI;

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

	from_label.text = _from.name;
	to_label.text = _to.name;

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

func _set_path_from_idx(idx: int) -> void:
	return _set_path_from_id(route_surface_opts.get_item_id(idx));

func _set_path_from_id(vehicle_id: int) -> void:
	var vehicle = vehicles[vehicle_id];
	_chosen_path = _to["onshores"] if vehicle.surface == Utils.TILE_SURFACE_GROUND else _to["offshores"]
	route_distance_label.text = "%dkm" % _chosen_path.size();
	# TODO update selected vehicle ui

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
	var vehicle = vehicles[route_surface_opts.get_item_id(route_surface_opts.selected)];
	dict["player"] = gui.player;
	dict["from"] = _from.name;
	dict["to"] = _to.name;
	dict["path"] = _chosen_path;
	dict["type"] = Route.ROUTE_CITY_CITY;
	dict["surface"] = vehicle.surface;
	dict["vehicle"] = vehicle.scene;
	stop_create();
	emit_signal("create_route", dict);
