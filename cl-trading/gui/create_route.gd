class_name CreateRoute extends Menu

signal create_route(ctx: Dictionary);

@onready var gui: GUI = $"../..";
@onready var from_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteConnection/FromLabel;
@onready var to_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteConnection/ToLabel;
@onready var route_distance_label: Label = $CreateRouteRect/CreateRouteContainer/RouteConnectionContainer/RouteDistanceLabel;
@onready var route_surface_opts: OptionButton = $CreateRouteRect/CreateRouteContainer/RouteSurfaceOptions;

var _from: City = null;
var _to: Dictionary = {};
var _destinations: Array = [];
var _chosen_path: Array = [];

func set_to(to: Entryable) -> void:
	var destination = null;
	for dest in _destinations:
		if dest["name"] == to.name:
			destination = dest;
			break;
	assert(destination != null, "destination is null");
	_to = destination;
	open_menu();

func open_menu() -> void:
	if visible:
		return;
	visible = true;

	from_label.text = _from.name;
	to_label.text = _to.name;

	if _to["onshores"].size() > 0:
		set_chosen_path(0);
		route_surface_opts.set_item_disabled(0, false);
		route_surface_opts.select(0);
	else:
		route_surface_opts.set_item_disabled(0, true);
	if _to["offshores"].size() > 0:
		route_surface_opts.set_item_disabled(1, false);
		if _chosen_path.size() == 0:
			set_chosen_path(1);
			route_surface_opts.select(1);
	else:
		route_surface_opts.set_item_disabled(1, true);

	gui.city_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

func set_chosen_path(index: int) -> void:
	_chosen_path = _to["onshores"] if index == 0 else _to["offshores"]
	route_distance_label.text = "%dkm" % _chosen_path.size();

func stop_create() -> void:
	visible = false;
	gui.is_creating_route = false;
	_from = null;
	_to = {};
	_destinations = [];
	_chosen_path = [];
	gui.city_manager.unlock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_city_menu_open_create_route_ui(from: City) -> void:
	gui.is_creating_route = true;
	_from = from;
	_destinations = gui.city_manager.get_cities_within_distance(from, gui.player.range);
	var names = [];
	for dest in _destinations:
		names.push_back(dest.name);
	gui.city_manager.lock_buttons_except(names);
	gui.camera_manager.unlock_cam();

func _on_close_menu() -> void:
	stop_create();

func _on_route_surface_options_item_selected(index: int) -> void:
	set_chosen_path(index);

func _on_route_cancel_btn_button_down() -> void:
	stop_create();

func _on_route_confirm_btn_button_down() -> void:
	var dict = {};
	dict["player"] = gui.player.name;
	dict["from"] = _from.name;
	dict["to"] = _to.name;
	dict["path"] = _chosen_path;
	dict["type"] = Route.ROUTE_CITY_CITY;
	dict["surface"] = route_surface_opts.get_item_id(route_surface_opts.selected);
	dict["vehicle"] = "vehicle_horse" if dict["surface"] == Utils.TILE_SURFACE_GROUND else "vehicle_ship";
	gui.player.get_connections().push_back(_to.name);
	#print("CREATING ROUTE: ", dict);
	stop_create();
	emit_signal("create_route", dict);
