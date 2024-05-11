class_name PlayerMenu extends Control

enum Tab {
	Routes = 0,
	Finance = 1,
	Resources = 2
}

const EMPTY_CARGO = -1;

@onready var container: TabContainer = $PlayerMenuContainer;
@onready var no_routes_label: Label = $PlayerMenuContainer/Routes/NoRoutesCreatedLabel;
@onready var routes_container: ScrollContainer = $PlayerMenuContainer/Routes/RoutesContainer;
@onready var routes_grid_container: GridContainer = $PlayerMenuContainer/Routes/RoutesContainer/RoutesGrid;

@export var gui: GUI;

var ctx: Dictionary = {
	Tab.Routes: {},
	Tab.Finance: {
		"cleanup": []
	},
	Tab.Resources: {
		"cleanup": []
	}
};

func _enter_tree() -> void:
	gui.route_manager.connect("onload-route-cargo", _on_onload_route_cargo);
	gui.route_manager.connect("offload-route-cargo", _on_offload_route_cargo);

func _exit_tree() -> void:
	gui.route_manager.disconnect("onload-route-cargo", _on_onload_route_cargo);
	gui.route_manager.disconnect("offload-route-cargo", _on_offload_route_cargo);

func _on_route_cargo_change(amount: int, player_name: StringName, route_name: StringName, kind: int) -> void:
	if !visible or player_name != gui.player.name:
		return;
	_set_cargo_item(route_name, kind, amount);

func _on_onload_route_cargo(player_name: StringName, route_name: StringName, kind: int) -> void:
	_on_route_cargo_change(1, player_name, route_name, kind);

func _on_offload_route_cargo(player_name: StringName, route_name: StringName, kind: int) -> void:
	_on_route_cargo_change(-1, player_name, route_name, kind);

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("open_player_menu") and !gui.is_creating_route and !gui.city_menu.visible:
		if visible:
			_close_menu();
		else:
			_open_menu();

func _open_menu(tab: int = Tab.Routes) -> void:
	visible = true;
	container.current_tab = tab;
	gui.city_manager.lock_all_buttons();
	gui.camera_manager.lock_cam();

	if tab == Tab.Routes:
		_render_route_tab();

func _ctx_add_route_item(route_name: String) -> void:
	ctx[Tab.Routes][route_name] = {
		"cleanup": [],
		"container": null,
		"cargo": {},
	};

func _ctx_cleanup_tab(tab: Tab) -> void:
	for fn in ctx[tab]["cleanup"]:
		fn.call();
	ctx[tab]["cleanup"] = [];

func _ctx_cleanup() -> void:
	_ctx_cleanup_tab(Tab.Finance);
	_ctx_cleanup_tab(Tab.Resources);
	for route in ctx[Tab.Routes].values():
		for fn in route["cleanup"]:
			fn.call();
		route["cleanup"] = [];
	ctx[Tab.Routes] = {};

func _render_route_tab() -> void:
		var routes = gui.route_manager.get_player_routes(gui.player.name);
		if routes.size() > 0:
			no_routes_label.visible = false;
			routes_container.visible = true;
			for route in routes:
				_create_route_entry(route);
		else:
			no_routes_label.visible = true;
			routes_container.visible = false;

func _create_route_entry(route: Route) -> void:
	_ctx_add_route_item(route.name);

	var cargo = _create_cargo_flow_container(route.name, route.get_current_cargo(), route.get_vehicle().cargo_space);
	var actions = _create_actions_flow_container();
	var origin = gui.create_label(route.get_start());
	var destination = gui.create_label(route.get_end());
	var profits = gui.create_label("%dg" % route.get_total_profits());

	var profits_changed_cb = func on_profits_changed(new_amount: int) -> void:
		profits.text = "%dg" % new_amount;

	route.connect("profits-changed", profits_changed_cb);

	routes_grid_container.add_child(origin);
	routes_grid_container.add_child(destination);
	routes_grid_container.add_child(profits);
	routes_grid_container.add_child(cargo);
	routes_grid_container.add_child(actions);

	ctx[Tab.Routes][route.name]["cleanup"].append(func clean_up() -> void:
		route.disconnect("profits-changed", profits_changed_cb);
		routes_grid_container.remove_child(origin);
		routes_grid_container.remove_child(destination);
		routes_grid_container.remove_child(profits);
		routes_grid_container.remove_child(cargo);
		routes_grid_container.remove_child(actions));

func _create_cargo_flow_container(route_name: String, cargo: Array[CityResource], space: int) -> FlowContainer:
	var flow_container = FlowContainer.new();
	ctx[Tab.Routes][route_name]["container"] = flow_container;
	flow_container.vertical = true;

	var used = 0;
	var i = 0;
	while used < space:
		var kind = EMPTY_CARGO;
		var amount = 1;
		if cargo.size() > i:
			var cargo_item: CityResource = cargo[i];
			kind = cargo_item.resource_kind;
			amount = cargo_item.amount;
		if ctx[Tab.Routes][route_name]["cargo"].has(kind):
			var target = ctx[Tab.Routes][route_name]["cargo"][kind];
			if kind == EMPTY_CARGO:
				used += 1;
				target["amount"] += 1;
			else:
				var resource = gui.base_resource_manager.get_resource(kind);
				target["amount"] += amount;
				used += (amount * resource.weight);
			target["node"].get_child(0).text = "%d" % target["amount"];
		elif amount > 0:
			var result = _create_cargo_flow_item(kind, amount);
			flow_container.add_child(result[0]);
			used += result[1];
			ctx[Tab.Routes][route_name]["cargo"][kind] = {
				"amount": amount,
				"node": result[0]
			};
		i += 1

	return flow_container;

func _set_cargo_item(route_name: String, kind: int, amount: int, adjust_empty_space: bool = true) -> void:
	if ctx[Tab.Routes][route_name]["cargo"].has(kind):
		var target = ctx[Tab.Routes][route_name]["cargo"][kind];
		var new_amount = target["amount"] + amount;
		if new_amount <= 0:
			ctx[Tab.Routes][route_name]["container"].remove_child(target["node"]);
			ctx[Tab.Routes][route_name]["cargo"].erase(kind);
		else:
			target["amount"] = new_amount;
			target["node"].get_child(0).text = "%d" % new_amount;
	else:
		var result = _create_cargo_flow_item(kind, amount);
		ctx[Tab.Routes][route_name]["container"].add_child(result[0]);
		ctx[Tab.Routes][route_name]["cargo"][kind] = {
			"amount": amount,
			"node": result[0]
		};
	if adjust_empty_space:
		var res = gui.base_resource_manager.get_resource(kind);
		_set_cargo_item(route_name, EMPTY_CARGO, -(amount * res.weight), false);

func _create_actions_flow_container() -> FlowContainer:
	var flow_container = FlowContainer.new();
	flow_container.vertical = true;
	flow_container.alignment = FlowContainer.ALIGNMENT_CENTER;

	var view_btn = Button.new();
	view_btn.text = "View";

	var edit_btn = Button.new();
	edit_btn.text = "Edit";

	var delete_btn = Button.new();
	delete_btn.text = "Delete";

	flow_container.add_child(view_btn);
	flow_container.add_child(edit_btn);
	flow_container.add_child(delete_btn);

	return flow_container;

func _create_cargo_flow_item(kind: int, amount: int) -> Array:
	var box = VBoxContainer.new();
	var count_label = gui.create_count(amount);
	var used_space = 1;
	count_label.add_theme_font_size_override("theme_override_font_sizes/font_size", 12);
	box.add_child(count_label);
	if kind == EMPTY_CARGO:
		var rect = ReferenceRect.new();
		rect.editor_only = false;
		rect.custom_minimum_size = Vector2(16, 16);
		rect.border_color = Color.WHITE;
		box.add_child(rect);
	else:
		var resource = gui.base_resource_manager.get_resource(kind);
		var icon = gui.base_resource_manager.get_resource_icon(kind);
		var texture = gui.create_texture(icon, Vector2(16, 16));
		box.add_child(texture);
		used_space = amount * resource.weight;
	return [box, used_space];

func _close_menu() -> void:
	container.current_tab = Tab.Routes;
	visible = false;
	_ctx_cleanup();
	gui.city_manager.unlock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_close_btn_button_down() -> void:
	_close_menu();
