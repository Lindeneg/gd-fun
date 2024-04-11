@tool
class_name Resources extends Node

var _icon_paths = {
	BaseResource.RESOURCE_PASSENGER: "res://assets/Icons/grain.png",
	BaseResource.RESOURCE_MAIL: "res://assets/Icons/grain.png",
	BaseResource.RESOURCE_GRAIN: "res://assets/Icons/grain.png",
	BaseResource.RESOURCE_WOOD: "res://assets/Icons/wood.png",
	BaseResource.RESOURCE_FISH: "res://assets/Icons/grain.png",
}

var _resources: Dictionary = {};

func get_resource(key: int) -> BaseResource:
	return _resources.get(key) as BaseResource;

func get_resource_icon_path(key: int):
	return _icon_paths.get(key);

func _ready() -> void:
	var children = get_children();
	for resource in children:
		_resources[resource.resource_kind] = resource
