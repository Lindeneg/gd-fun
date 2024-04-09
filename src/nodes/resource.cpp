#include "resource.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "../core/utils.h"
#include "./base_resource.h"

// SIGNALS
const char *godot::CL::ResourceTile::SAmountChanged{"amount_changed"};

godot::CL::ResourceTile::ResourceTile()
    : Entryable(),
      resource_kind_(RESOURCE_PASSENGER),
      capacity_amount_(0),
      current_amount_(0) {}

godot::CL::ResourceTile::~ResourceTile() {}

void godot::CL::ResourceTile::_ready() {
    if (Utils::is_in_editor()) {
        e_assign_required_components_();
    } else {
        r_assign_required_components_();
    }
    set_y_sort_enabled(true);
    set_z_index(5);
    set_monitorable(false);
    set_collision_layer(COLLISION_LAYER_RESOURCE);
    set_collision_mask(COLLISION_LAYER_VEHICLE);
}

void godot::CL::ResourceTile::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_resource_kind"),
                         &ResourceTile::get_resource_kind);
    ClassDB::bind_method(D_METHOD("set_resource_kind", "k"),
                         &ResourceTile::set_resource_kind);

    ClassDB::bind_method(D_METHOD("get_capacity_amount"),
                         &ResourceTile::get_capacity_amount);
    ClassDB::bind_method(D_METHOD("set_capacity_amount", "c"),
                         &ResourceTile::set_capacity_amount);

    ClassDB::bind_method(D_METHOD("get_current_amount"),
                         &ResourceTile::get_current_amount);
    ClassDB::bind_method(D_METHOD("set_current_amount", "c"),
                         &ResourceTile::set_current_amount);

    ClassDB::add_property("ResourceTile",
                          PropertyInfo(Variant::INT, "capacity_amount",
                                       PROPERTY_HINT_RANGE, "1,500"),
                          "set_capacity_amount", "get_capacity_amount");

    ClassDB::add_property("ResourceTile",
                          PropertyInfo(Variant::INT, "start_amount",
                                       PROPERTY_HINT_RANGE, "1,500"),
                          "set_current_amount", "get_current_amount");

    ClassDB::add_property(
        "ResourceTile",
        PropertyInfo(Variant::INT, "resource_kind", PROPERTY_HINT_ENUM,
                     "Passenger:0,Mail:1,Grain:2,Fish:3,Rock:4,Iron:5,Wood:6,"
                     "Boar:7,Pig:8,Chicken:9,Sheep:10,Poppy:11,Food:12,Beer:13,"
                     "Cobble:14,Tools:15,Textiles:16,Medicine:17"),
        "set_resource_kind", "get_resource_kind");

    ClassDB::add_signal(
        "ResourceTile",
        MethodInfo(SAmountChanged, PropertyInfo(Variant::INT, "amount")));
}
