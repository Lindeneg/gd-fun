@tool
class_name GUI extends Control

signal create_route(ctx: Dictionary);

const _down_arrow_texture = preload("res://assets/Icons/down-arrow.png");

const _city_ui_scene = preload("res://gui/city_ui.tscn");
const _resource_ui_scene = preload("res://gui/resource_ui.tscn");

@export var camera_manager: CameraManager;
@export var city_manager: CityManager;
@export var tile_manager: TileManager;
@export var route_manager: RouteManager;
@export var resource_manager: ResourceManager;
@export var debug_manager: DebugManager;
@export var base_resource_manager: BaseResourceManager;
@export var player: Player;

@onready var city_menu: CityMenu = $GUIContainer/CityMenu;
@onready var create_route_ui: CreateRoute = $GUIContainer/CreateRouteMenu;
@onready var container: CanvasLayer = $GUIContainer
@onready var player_ctx: Panel = $GUIContainer/PlayerCtx;
@onready var top_right: Control = $GUIContainer/TopRight;
@onready var gold_label: Label = $GUIContainer/PlayerCtx/PlayerCtxContainer/GoldLabel;

var player_finance: PlayerFinance;
var is_creating_route: bool = false;

func _ready() -> void:
	visible = true;
	container.visible = true;
	_setup_uis();
	if !Engine.is_editor_hint():
		resource_manager.lock_all_buttons();
		top_right.visible = true;
		player_ctx.visible = true;
		player_finance = player.get_finance();
		player_finance.connect("gold-changed", _on_player_gold_changed);
		_on_player_gold_changed(player_finance.get_gold_());

func _setup_uis() -> void:
	if !city_manager or !resource_manager:
		return;
	var cities = city_manager.get_cities();
	for key in cities.keys():
		var city_ui = _city_ui_scene.instantiate();
		var city: City = cities[key];
		city_ui.player = player;
		city_ui.resources = base_resource_manager;
		city_ui.city = city;
		city.add_child(city_ui);

	var managed_resources = resource_manager.get_resources();
	for key in managed_resources.keys():
		var resource_ui = _resource_ui_scene.instantiate();
		var resource: ResourceTile = managed_resources[key];
		resource_ui.custom_minimum_size = Vector2(25, 25);
		resource_ui.anchors_preset = PRESET_BOTTOM_LEFT;
		resource_ui.player = player;
		resource_ui.resources = base_resource_manager;
		resource_ui.resource = resource;
		resource.add_child(resource_ui);

func remove_nodes_children(nodes: Array) -> void:
	for node in nodes:
		remove_node_children(node);

func set_create_route_to(entry: Entryable) -> void:
	if is_creating_route:
		create_route_ui.set_to(entry);

func create_supply_item(resource_kind: int, amount: int, node: Node) -> void:
	var supply_item = VBoxContainer.new();
	var supply_count = create_count(amount);
	var supply_texture = create_texture(base_resource_manager.get_resource_icon(resource_kind));
	var resource = base_resource_manager.get_resource(resource_kind);

	supply_item.name = resource.name;
	supply_item.add_child(supply_count);
	supply_item.add_child(supply_texture);
	node.add_child(supply_item);

func create_route_supply_item(resource_kind: int, node: Node, cb: Callable, color: Color = Color.DARK_GREEN) -> int:
	var supply_item = ReferenceRect.new();
	var supply_btn = TextureButton.new();
	var resource_size = 1;

	if resource_kind > -1:
		var resource = base_resource_manager.get_resource(resource_kind);
		supply_btn.texture_normal = base_resource_manager.get_resource_icon(resource_kind);
		supply_item.name = resource.name;
		supply_item.tooltip_text = "Weight: %d" % resource.weight;
		resource_size = resource.weight;

	supply_btn.stretch_mode = TextureButton.STRETCH_SCALE;
	supply_btn.custom_minimum_size = Vector2(32, 32);
	supply_btn.mouse_filter = Control.MOUSE_FILTER_PASS;
	supply_btn.connect("button_down", cb);

	supply_item.editor_only = false;
	supply_item.mouse_filter = Control.MOUSE_FILTER_PASS;
	supply_item.mouse_default_cursor_shape = Control.CURSOR_POINTING_HAND;
	supply_item.custom_minimum_size = Vector2(32, 32);
	supply_item.border_color = color;
	supply_item.add_child(supply_btn);
	node.add_child(supply_item);
	return resource_size;

func create_demand_item(resource_kind: int, node: Node) -> void:
	var demand_item = VBoxContainer.new();
	var demand_texture = create_texture(base_resource_manager.get_resource_icon(resource_kind));
	var resource = base_resource_manager.get_resource(resource_kind);

	demand_item.name = resource.name;
	demand_item.add_child(demand_texture);
	node.add_child(demand_item);

func create_industry_item(industry: Industry, node: Node) -> void:
	var industry_item = VBoxContainer.new();
	var industry_in_texture = create_texture(base_resource_manager.get_resource_icon(industry.in));
	var needed_count = create_count(industry.conversion);
	var industry_arrow_texture = create_texture(_down_arrow_texture);
	var industry_out_texture = create_texture(base_resource_manager.get_resource_icon(industry.out));

	industry_item.add_child(industry_in_texture);
	industry_item.add_child(needed_count);
	industry_item.add_child(industry_arrow_texture);
	industry_item.add_child(industry_out_texture);
	node.add_child(industry_item);

func create_texture(texture: Object, item_size: Vector2 = Vector2(32, 32)) -> TextureRect:
	var item = TextureRect.new();
	item.texture = texture;
	item.custom_minimum_size = item_size;
	return item;

func create_label(txt: String) -> Label:
	var item = Label.new();
	item.text = txt;
	return item;

func create_count(count: int) -> Label:
	var item = create_label("%d" % count);
	item.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER;
	return item;

func remove_node_children(node: Node) -> void:
	var children = node.get_children();
	for child in children:
		node.remove_child(child);
		child.queue_free();

func _on_resource_manager_resource_clicked(resource_name: StringName) -> void:
	if is_creating_route:
		create_route_ui.set_to(resource_manager.get_resource(resource_name));

func _on_city_manager_city_clicked(city_name: StringName) -> void:
	if !city_manager:
		return;
	city_menu.open_menu(city_manager.get_city(city_name));

func _on_player_gold_changed(new_amount: int) -> void:
	gold_label.text = "%dg" % new_amount;
	create_route_ui.player_gold_changed(new_amount);

func _on_create_route_menu_create_route(ctx: Dictionary) -> void:
		emit_signal("create_route", ctx);
