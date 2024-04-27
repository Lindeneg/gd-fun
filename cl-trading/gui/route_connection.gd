class_name RouteConnection extends HBoxContainer

enum ArrowDirection {
	Right,
	Left,
	Both
}

const _textures = {
	ArrowDirection.Right: preload("res://assets/Icons/right-arrow.png"),
	ArrowDirection.Left: preload("res://assets/Icons/left-arrow.png"),
	ArrowDirection.Both: preload("res://assets/Icons/connection.png"),
}

@export var direction: ArrowDirection = ArrowDirection.Both:
	get:
		return direction;
	set(value):
		direction = value;
		_update_arrow_direction();


@onready var from_label: Label = $FromLabel;
@onready var to_label: Label = $ToLabel;
@onready var connection_rect: TextureRect = $ConnectionTexture;

func _update_arrow_direction() -> void:
	if !connection_rect:
		return;
	connection_rect.texture = _textures[direction];
