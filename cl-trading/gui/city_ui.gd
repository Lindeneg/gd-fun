@tool
extends ColorRect

enum ResourceKind {
	SUPPLY,
	DEMAND
}

const BASE_ITEM_SIZE: float = 23.3;
const BASE_UI_HEIGHT: int = 50;

@onready var supply = $SupplyContainer;
@onready var demand = $DemandContainer;

var city_resource_icon = preload("res://gui/city_resource_icon.tscn");

var city: City;
var resources: Resources;

func _ready() -> void:
	var nullable_city = get_node_or_null("..");
	var nullable_resources = get_node_or_null("../../../BaseResources");
	if !nullable_resources or !nullable_city:
		return;
	city = nullable_city;
	resources = nullable_resources;
	position.y = -BASE_UI_HEIGHT;
	size.x = BASE_ITEM_SIZE * city.supplies.size();
	size.y = BASE_UI_HEIGHT;
	for cs in city.supplies:
		create_supply_demand(cs, ResourceKind.SUPPLY);
	for cd in city.demands:
		create_supply_demand(cd, ResourceKind.DEMAND);

func create_supply_demand(cr: CityResource, kind: ResourceKind) -> void:
	var city_resource = create_city_resource(cr);
	if !city_resource:
		return;
	var icon_path = get_icon_path(cr);
	if !icon_path:
		return;
	if kind == ResourceKind.SUPPLY:
		city_resource.create_supply(icon_path, cr.amount);
		supply.add_child(city_resource);
	elif kind == ResourceKind.DEMAND:
		city_resource.create_demand(icon_path);
		demand.add_child(city_resource);

func get_icon_path(cr: CityResource) -> String:
	var icon_path = resources.get_resource_icon_path(cr.resource_kind);
	if !icon_path:
		printerr("failed to find icon path for resource %d" % cr.resource_kind);
		return "";
	return icon_path;

func create_city_resource(cr: CityResource) -> CityResourceIcon:
	if !city_resource_icon.can_instantiate():
		printerr("failed to instanciate icon for resource %d" % cr.resource_kind);
		return null;
	var city_resource = city_resource_icon.instantiate() as CityResourceIcon;
	return city_resource;
