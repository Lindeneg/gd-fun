class_name PlayerMenu extends Control

@onready var container: TabContainer = $PlayerMenuContainer;
@onready var no_routes_label: Label = $PlayerMenuContainer/Routes/NoRoutesCreatedLabel;
@onready var routes_container: ScrollContainer = $PlayerMenuContainer/Routes/RoutesContainer;
@onready var routes_grid_container: GridContainer = $PlayerMenuContainer/Routes/RoutesContainer/RoutesGrid;

@export var gui: GUI;

var _clean_up_fns = [];

enum Tab {
	Routes = 0,
	Finance = 1,
	Resources = 2
}

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
	var cargo = _create_cargo_flow_container(route.get_current_cargo(), route.get_vehicle().cargo_space);
	var actions = _create_actions_flow_container();
	var origin = gui.create_label(route.get_start());
	var destination = gui.create_label(route.get_end());
	var profits = gui.create_label("%dg" % route.get_total_profits());

	routes_grid_container.add_child(origin);
	routes_grid_container.add_child(destination);
	routes_grid_container.add_child(profits);
	routes_grid_container.add_child(cargo);
	routes_grid_container.add_child(actions);

	_clean_up_fns.append(Callable(self, "_cleanup_route_entry").bind(
		origin,
		destination,
		profits,
		cargo,
		actions
	));

func _cleanup_route_entry(origin: Label, destination: Label, profits: Label, cargo: FlowContainer, actions: FlowContainer) -> void:
	routes_grid_container.remove_child(origin);
	routes_grid_container.remove_child(destination);
	routes_grid_container.remove_child(profits);
	routes_grid_container.remove_child(cargo);
	routes_grid_container.remove_child(actions);

func _create_cargo_flow_container(cargo: Array[CityResource], space: int) -> FlowContainer:
	var flow_container = FlowContainer.new();
	flow_container.vertical = true;

	var used = 0;
	var i = 0;
	while used < space:
		var kind = -1;
		var amount = 0;
		if cargo.size() > i:
			var cargo_item: CityResource = cargo[i];
			kind = cargo_item.resource_kind;
			amount = cargo_item.amount;
		var result = _create_cargo_flow_item(kind, amount);
		flow_container.add_child(result[0]);
		used += result[1];
		i += 1

	return flow_container;

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
	if amount == 0:
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
	for fn in _clean_up_fns:
		fn.call();
	_clean_up_fns = [];
	gui.city_manager.unlock_all_buttons();
	gui.camera_manager.unlock_cam();

func _on_close_btn_button_down() -> void:
	_close_menu();
