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
@export var resources: Resources;
@export var player: Player;

@onready var city_menu: CityMenu = $GUIContainer/CityMenu;
@onready var create_route_ui: CreateRoute = $GUIContainer/CreateRoute;
@onready var container: CanvasLayer = $GUIContainer;
var is_creating_route: bool = false;

func _ready() -> void:
	visible = true;
	container.visible = true;
	_setup_uis();
	if !Engine.is_editor_hint():
		resource_manager.lock_all_buttons();

func _setup_uis() -> void:
	var cities = city_manager.get_cities();
	for key in cities.keys():
		var city_ui = _city_ui_scene.instantiate();
		var city: City = cities[key];
		city_ui.player = player;
		city_ui.resources = resources;
		city_ui.city = city;
		city.add_child(city_ui);

	var managed_resources = resource_manager.get_resources();
	for key in managed_resources.keys():
		var resource_ui = _resource_ui_scene.instantiate();
		var resource: ResourceTile = managed_resources[key];
		resource_ui.custom_minimum_size = Vector2(25, 25);
		resource_ui.anchors_preset = PRESET_BOTTOM_LEFT;
		resource_ui.player = player;
		resource_ui.resources = resources;
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
	var supply_texture = create_texture(resources.get_resource_icon(resource_kind));
	var resource = resources.get_resource(resource_kind);

	supply_item.name = resource.name;
	supply_item.add_child(supply_count);
	supply_item.add_child(supply_texture);
	node.add_child(supply_item);

func create_demand_item(resource_kind: int, node: Node) -> void:
	var demand_item = VBoxContainer.new();
	var demand_texture = create_texture(resources.get_resource_icon(resource_kind));
	var resource = resources.get_resource(resource_kind);

	demand_item.name = resource.name;
	demand_item.add_child(demand_texture);
	node.add_child(demand_item);

func create_industry_item(industry: Industry, node: Node) -> void:
	var industry_item = VBoxContainer.new();
	var industry_in_texture = create_texture(resources.get_resource_icon(industry.in));
	var needed_count = create_count(industry.conversion);
	var industry_arrow_texture = create_texture(_down_arrow_texture);
	var industry_out_texture =create_texture(resources.get_resource_icon(industry.out));

	industry_item.add_child(industry_in_texture);
	industry_item.add_child(needed_count);
	industry_item.add_child(industry_arrow_texture);
	industry_item.add_child(industry_out_texture);
	node.add_child(industry_item);

func create_texture(texture: Object) -> TextureRect:
	var item = TextureRect.new();
	item.texture = texture;
	item.custom_minimum_size = Vector2(32, 32);
	return item;

func create_count(count: int) -> Label:
	var item = Label.new();
	item.text = "%d" % count;
	item.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER;
	return item;

func remove_node_children(node: Node) -> void:
	var children = node.get_children();
	for child in children:
		node.remove_child(child);
		child.free();

func _on_create_route_create_route(ctx: Dictionary) -> void:
	emit_signal("create_route", ctx);

func _on_resource_manager_resource_clicked(resource_name: StringName) -> void:
	if is_creating_route:
		create_route_ui.set_to(resource_manager.get_resource(resource_name));

func _on_city_manager_city_clicked(city_name: StringName) -> void:
	if !city_manager:
		return;
	city_menu.open_menu(city_manager.get_city(city_name));
