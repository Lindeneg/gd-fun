@tool
extends Control

@export var y_btn_offset: float = 32;

@onready var btn_layer: CanvasLayer = $BtnLayer;
@onready var btn: Button = $BtnLayer/NameBtn;
@onready var count_label: Label = $ColorRect/ResourceCount;

var resource: ResourceTile;
var player: Player;
var resources: Resources;
var base_resource: BaseResource;

func _ready() -> void:
	if !resource or !player or !resources:
		btn_layer.visible = false;
		return;
	base_resource = resources.get_resource(resource.resource_kind);
	resource.connect("btn_state_changed", _on_btn_state_changed);
	btn_layer.visible = true;
	if !Engine.is_editor_hint():
		z_index = 1;
	count_label.text = "%d" % resource.get_current_amount();
	btn_layer.transform = Transform2D(0, Vector2(resource.global_position.x, resource.global_position.y + y_btn_offset));
	btn.text = base_resource.name;
	btn.icon = resources.get_resource_icon(resource.resource_kind);

func _on_btn_state_changed(enabled: bool) -> void:
	btn.disabled = !enabled;
	if enabled:
		btn.mouse_default_cursor_shape = CURSOR_POINTING_HAND;
	else:
		btn.mouse_default_cursor_shape = CURSOR_FORBIDDEN;

func _on_name_btn_button_down() -> void:
	resource.emit_signal("btn_clicked", resource.name);
