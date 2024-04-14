@tool
class_name CityResourceIcon extends TextureRect

func create_supply(icon: Object, amount: int) -> void:
	update_texture(icon);
	var label = get_child(0);
	label.text = "%d" % amount;

func create_demand(icon: Object) -> void:
	update_texture(icon);
	var label = get_child(0);
	remove_child(label);

func update_texture(icon: Object) -> void:
	texture = icon;
	custom_minimum_size = Vector2(24, 24);
	expand_mode = TextureRect.EXPAND_KEEP_SIZE;
	stretch_mode = TextureRect.STRETCH_SCALE;
