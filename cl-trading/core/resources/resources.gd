@tool
class_name Resources extends Node

var _icon_paths = {
	BaseResource.RESOURCE_PASSENGER: preload("res://assets/Icons/passenger.png"),
	BaseResource.RESOURCE_MAIL: preload("res://assets/Icons/mail.png"),
	BaseResource.RESOURCE_GRAIN: preload("res://assets/Icons/grain.png"),
	BaseResource.RESOURCE_WOOD: preload("res://assets/Icons/wood.png"),
	BaseResource.RESOURCE_FISH: preload("res://assets/Icons/grain.png"),
	BaseResource.RESOURCE_FOOD: preload("res://assets/Icons/food.png"),
	BaseResource.RESOURCE_TOOLS: preload("res://assets/Icons/wood.png")
}

var _resources: Dictionary = {};

func _ready() -> void:
	var children = get_children();
	for resource in children:
		_resources[resource.resource_kind] = resource

func get_resource(key: int) -> BaseResource:
	return _resources.get(key) as BaseResource;

func get_resource_icon(key: int):
	return _icon_paths.get(key);
