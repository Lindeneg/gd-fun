@tool
extends ColorRect

enum ResourceType {
	SUPPLY,
	DEMAND
}

const BASE_ITEM_SIZE: float = 23.3;
const BASE_UI_HEIGHT: int = 50;

@export var y_container_offset: float = 0.0;
@export var y_btn_offset: float = 32;

@onready var supply = $SupplyContainer;
@onready var demand = $DemandContainer;
@onready var btn_layer = $BtnLayer;
@onready var btn = $BtnLayer/NameBtn;

var city_resource_icon = preload("res://gui/city_resource_icon.tscn");

var city: City;
var resources: Resources;

func _ready() -> void:
	# set required members
	var nullable_city = get_node_or_null("..");
	var nullable_resources = get_node_or_null("../../../BaseResources");
	if !nullable_resources or !nullable_city:
		btn_layer.visible = false;
		return;
	city = nullable_city;
	city.connect("btn_state_changed", _on_btn_state_changed);
	resources = nullable_resources;
	btn_layer.visible = true;
	# adjust container size
	position.y = -(BASE_UI_HEIGHT + y_container_offset);
	size.x = BASE_ITEM_SIZE * _get_visual_tile_width();
	size.y = BASE_UI_HEIGHT;
	btn_layer.transform = Transform2D(0, Vector2(city.global_position.x, city.global_position.y + (y_btn_offset - y_container_offset)));
	btn.text = city.name;
	# display city resources
	for cs in city.supplies:
		_create_supply_demand(cs.resource_kind, cs.amount, ResourceType.SUPPLY);
	for cd in city.demands:
		_create_supply_demand(cd.resource_kind, 0, ResourceType.DEMAND);
	for industry in city.industries:
		_create_supply_demand(industry.out, industry.amount, ResourceType.SUPPLY);
		_create_supply_demand(industry.in, 0, ResourceType.DEMAND);

func _create_supply_demand(resource_kind: int, amount: int, type: ResourceType) -> void:
	if !city_resource_icon.can_instantiate():
		printerr("failed to instanciate icon for resource %d" % resource_kind);
		return;
	var city_resource = city_resource_icon.instantiate() as CityResourceIcon;
	var icon = resources.get_resource_icon(resource_kind);
	if !icon:
		printerr("failed to find icon for resource %d" % resource_kind);
		return;
	if type == ResourceType.SUPPLY:
		city_resource.create_supply(icon, amount);
		supply.add_child(city_resource);
	elif type == ResourceType.DEMAND:
		city_resource.create_demand(icon);
		demand.add_child(city_resource);

func _get_visual_tile_width() -> int:
	var supplies = city.supplies.size();
	var demands = city.demands.size();
	var industries = city.industries.size();
	if supplies >= demands:
		return supplies + industries;
	return demands + industries;

func _on_btn_state_changed(enabled: bool) -> void:
	btn.disabled = !enabled;
	if enabled:
		btn.mouse_default_cursor_shape = CURSOR_POINTING_HAND;
	else:
		btn.mouse_default_cursor_shape = CURSOR_FORBIDDEN;

func _on_name_btn_button_down() -> void:
	print("CLICK CITY UI")
	city.emit_signal("btn_clicked", city.name);
