extends TradingVehicle

func _process(delta: float) -> void:
	handle_movement(delta);
	if debug_mode:
		queue_redraw();

func _draw() -> void:
	if is_moving():
		draw_line(to_local(position), to_local(get_navigation_target()), Color.RED);
