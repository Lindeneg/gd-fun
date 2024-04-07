extends Node

var resources: Dictionary = {};

func get_resource(key: int) -> BaseResource:
	return resources.get(key) as BaseResource;

func _ready() -> void:
	var children = get_children();
	for resource in children:
		resources[resource.resource_kind] = resource
