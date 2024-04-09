extends Container

@onready var camera_manager: Camera2D = $"../..";
@onready var cam_label: Label = $CamLabel;
@onready var cam_timer: Timer = $CamLabel/CamLabelTimer;

func _ready() -> void:
	cam_label.visible = false;
	cam_label.text = "Cam Speed: %.1f" % camera_manager.base_move_speed;

func _on_camera_manager_cam_speed_changed(new_speed: float) -> void:
	cam_label.text = "Cam Speed: %.1f" % new_speed;
	cam_label.visible = true;
	cam_timer.start();

func _on_cam_label_timer_timeout() -> void:
	cam_label.visible = false;
