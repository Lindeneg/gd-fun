@tool
extends BaseResourceManager

var _icon_paths = {
	BaseResource.RESOURCE_PASSENGER: preload("res://assets/Icons/passenger.png"),
	BaseResource.RESOURCE_MAIL: preload("res://assets/Icons/mail.png"),
	BaseResource.RESOURCE_GRAIN: preload("res://assets/Icons/grain.png"),
	BaseResource.RESOURCE_WOOD: preload("res://assets/Icons/wood.png"),
	BaseResource.RESOURCE_FISH: preload("res://assets/Icons/grain.png"),
	BaseResource.RESOURCE_FOOD: preload("res://assets/Icons/food.png"),
	BaseResource.RESOURCE_TOOLS: preload("res://assets/Icons/wood.png")
}

func get_resource_icon(key: int):
	return _icon_paths.get(key);
