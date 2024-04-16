class_name GUI extends Control

const _down_arrow_texture = preload("res://assets/Icons/down-arrow.png");

@export var camera_manager: Camera2D;
@export var city_manager: CityManager;
@export var route_manager: RouteManager;
@export var resources: Resources;

@onready var city_menu: CityMenu = $GUIContainer/CityMenu;
@onready var container: CanvasLayer = $GUIContainer;

func _ready() -> void:
	visible = true;
	container.visible = true;

func _on_city_manager_city_clicked(city_name: StringName) -> void:
	if !city_manager:
		return;
	city_menu.open_menu(city_manager.get_city(city_name));

func remove_nodes_children(nodes: Array) -> void:
	for node in nodes:
		remove_node_children(node);

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
