@tool
extends Control

@export var y_btn_offset: float = 32;

@onready var btn_layer: CanvasLayer = $BtnLayer;
@onready var btn: Button = $BtnLayer/NameBtn;
@onready var count_label: Label = $ColorRect/ResourceCount;

var _gui: GUI;
var _resource: ResourceTile;
var _base_resource: BaseResource;

func _ready() -> void:
	var nullable_resource = get_node_or_null("..");
	var nullable_gui = get_node_or_null("../../../PlayerManager/PlayerLars/CameraManager/GUI");
	if !nullable_gui or !nullable_resource:
		btn_layer.visible = false;
		return;
	_resource = nullable_resource;
	_gui = nullable_gui;
	_base_resource = _gui.resources.get_resource(_resource.resource_kind);
	_resource.connect("btn_state_changed", _on_btn_state_changed);
	btn_layer.visible = true;
	if !Engine.is_editor_hint():
		z_index = 1;
	count_label.text = "%d" % _resource.get_current_amount();
	btn_layer.transform = Transform2D(0, Vector2(_resource.global_position.x, _resource.global_position.y + y_btn_offset));
	btn.text = _base_resource.name;
	btn.icon = _gui.resources.get_resource_icon(_resource.resource_kind);

func _on_btn_state_changed(enabled: bool) -> void:
	btn.disabled = !enabled;
	if enabled:
		btn.mouse_default_cursor_shape = CURSOR_POINTING_HAND;
	else:
		btn.mouse_default_cursor_shape = CURSOR_FORBIDDEN;

func _on_name_btn_button_down() -> void:
	_resource.emit_signal("btn_clicked", _resource.name);
