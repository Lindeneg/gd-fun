@tool
extends ColorRect

enum ResourceType {
	SUPPLY,
	DEMAND
}

const BASE_ITEM_SIZE: float = 23.3;
const BASE_UI_HEIGHT: int = 50;

@onready var supply = $SupplyContainer;
@onready var demand = $DemandContainer;
@onready var btn_layer = $BtnLayer;
@onready var btn = $BtnLayer/NameBtn;

var city_resource_icon = preload("res://gui/city_resource_icon.tscn");

var city: City;
var player: Player;
var resources: Resources;

func _ready() -> void:
	if !city or !player or !resources:
		btn_layer.visible = false;
		return;
	city.connect("btn_state_changed", _on_btn_state_changed);
	btn_layer.visible = true;
	# adjust container size
	if !Engine.is_editor_hint():
		z_index = 1;
	position.y = -(BASE_UI_HEIGHT + city.y_container_offset);
	size.x = BASE_ITEM_SIZE * _get_visual_tile_width();
	size.y = BASE_UI_HEIGHT;
	btn_layer.transform = Transform2D(0, Vector2(city.global_position.x, city.global_position.y + (city.y_btn_offset - city.y_container_offset)));
	btn.text = city.name;
	if player.start_city == city.name || player.get_connections().has(city.name):
		btn.text = "%s*" % city.name;
	else:
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
	city_resource.name = resources.get_resource(resource_kind).name;
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
	city.emit_signal("btn_clicked", city.name);
