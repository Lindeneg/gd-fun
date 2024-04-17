class_name Menu extends Control

signal close_menu();

var _focused: bool = false;

func _ready() -> void:
	visible = false;
	connect("mouse_entered", _on_mouse_entered);
	connect("mouse_exited", _on_mouse_exited);

func _input(event: InputEvent) -> void:
	if !visible:
		return
	if !_focused and event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		emit_signal("close_menu");

func _on_mouse_entered() -> void:
	_focused = true;

func _on_mouse_exited() -> void:
	_focused = false;
