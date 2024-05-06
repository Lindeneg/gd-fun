"""
RouteManager knows everything about how to create, update and delete Routes.
"""
extends RouteManager

const _vehicle_scenes = [
	"res://sprites/vehicles/onshore/vehicle_horse.tscn",
	"res://sprites/vehicles/offshore/vehicle_ship.tscn"
];

var vehicles = [];

func _init() -> void:
	for scene in _vehicle_scenes:
		var loaded_scene = load(scene) as PackedScene;
		var vehicle = loaded_scene.instantiate() as TradingVehicle;
		var ctx = {
			"name": vehicle.name,
			"scene": loaded_scene,
			"tier": vehicle.tier,
			"surface": vehicle.vehicle_surface,
			"speed": vehicle.speed,
			"cargo_space": vehicle.cargo_space,
			"cost": vehicle.cost,
			"upkeep": vehicle.upkeep
		}
		vehicles.append(ctx);
		vehicle.free();

func _increment_cargo_if_exists(cargo: Array, kind: int) -> bool:
	for c in cargo:
		if c.resource_kind == kind:
			c.capacity += 1;
			return true;
	return false;

func _update_cargo_dict(cargo: Array, kinds: Array) -> void:
	for kind in kinds:
		if (_increment_cargo_if_exists(cargo, kind)):
			continue;
		var r = CityResource.new();
		r.resource_kind = kind;
		r.capacity = 1;
		r.amount = 0;
		cargo.append(r);

func _on_gui_create_route(ctx: Dictionary) -> void:
	var vehicle = ctx["vehicle"].instantiate() as TradingVehicle;
	if !vehicle:
		return;

	var route: Route = Route.new();

	route.set_player(ctx["player"]);
	route.set_type(ctx["surface"]);
	route.set_kind(ctx["type"]);
	route.set_start(ctx["from"]);
	route.set_end(ctx["to"]);
	route.set_current_route(ctx["path"]);
	route.set_vehicle(vehicle);
	route.set_debug(debug);

	var cargo = {
		TradingVehicle.VEHICLE_MOVE_DIR_AB: [],
		TradingVehicle.VEHICLE_MOVE_DIR_BA: [],
	};

	if ctx["type"] == Entryable.ENTRYABLE_CITY:
		_update_cargo_dict(cargo[TradingVehicle.VEHICLE_MOVE_DIR_AB], ctx["cargo"][TradingVehicle.VEHICLE_MOVE_DIR_AB]["cargo"]);
		_update_cargo_dict(cargo[TradingVehicle.VEHICLE_MOVE_DIR_BA], ctx["cargo"][TradingVehicle.VEHICLE_MOVE_DIR_BA]["cargo"]);
	else:
		var r = CityResource.new();
		r.resource_kind = ctx["res"];
		r.capacity = vehicle.cargo_space;
		r.amount = 0;
		cargo[TradingVehicle.VEHICLE_MOVE_DIR_BA].append(r);

	route.set_cargo(cargo);
	add_route(route);
