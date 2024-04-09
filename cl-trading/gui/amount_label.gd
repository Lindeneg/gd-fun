@tool
extends Label
@onready var tile = $".." as ResourceTile;

func _ready() -> void:
	if tile:
		tile.connect("amount_changed", on_amount_changed);
		text = str(tile.start_amount);

func on_amount_changed(amount: int) -> void:
	text = str(amount);
