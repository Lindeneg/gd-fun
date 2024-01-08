extends HBoxContainer

signal create_debug_route(c1: String, c2: String, surface: int);
signal remove_debug_route(route_name: String);

@onready var city1_opts = $Option1;
@onready var city2_opts = $Option2;
@onready var surface_type = $Option3;
@onready var create_btn = $CreateRouteBtn;

var city_nodes = [];
var created_routes = [];

func add_city_nodes(nodes: Array) -> void:
	if !nodes:
		return;
	city_nodes = nodes;
	for city in city_nodes:
		city1_opts.add_item(city.name);
		city2_opts.add_item(city.name);

func add_city_node(node: Node) -> void:
	city1_opts.add_item(node.name);
	city2_opts.add_item(node.name);

func get_selected_text(btn: OptionButton) -> String:
	return btn.get_item_text(btn.selected);

func get_selected_id(btn: OptionButton) -> int:
	return btn.get_item_id(btn.selected);

func _ready() -> void:
	create_btn.disabled = true;

func _process(_delta: float) -> void:
	create_btn.disabled = city1_opts.selected == city2_opts.selected;

func _on_create_route_btn_button_down() -> void:
	emit_signal(
		"create_debug_route",
		get_selected_text(city1_opts),
		get_selected_text(city2_opts),
		get_selected_id(surface_type)
	);
