#include "resource_tile.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "../core/utils.h"
#include "./base_resource.h"

#ifdef CL_TRADING_DEBUG
MAKE_LOG(RESLOG, ResourceTile)
#endif

// SIGNALS
const char *godot::CL::ResourceTile::SAmountChanged{"amount_changed"};

godot::CL::ResourceTile::ResourceTile()
    : Entryable(ENTRYABLE_RESOURCE),
      resource_kind_(RESOURCE_PASSENGER),
      capacity_amount_(0),
      current_amount_(0) {}

godot::CL::ResourceTile::~ResourceTile() {}

void godot::CL::ResourceTile::on_restock_timeout_() {
    int requested_amount{get_restock_amount()};
    int new_amount{current_amount_ + requested_amount};
    if (new_amount <= capacity_amount_) {
        set_current_amount(new_amount);
#ifdef CL_TRADING_DEBUG
        RESLOG(this, "has restocked %d of resource %d\n", requested_amount,
               resource_kind_);
#endif
    }
}

void godot::CL::ResourceTile::_ready() {
    if (Utils::is_in_editor()) {
        e_assign_required_components_();
    } else {
        r_assign_required_components_(get_restock_timeout());
    }
    set_y_sort_enabled(true);
    set_z_index(5);
    set_monitorable(false);
    set_collision_layer(COLLISION_LAYER_RESOURCE);
    set_collision_mask(COLLISION_LAYER_VEHICLE);
    start_restock_timer();
}

void godot::CL::ResourceTile::_bind_methods() {
    // BIND METHODS
    DEBUG_BIND(ResourceTile)

    ClassDB::bind_method(D_METHOD("get_size"), &ResourceTile::get_size);
    ClassDB::bind_method(D_METHOD("set_size", "s"), &ResourceTile::set_size);

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

    ClassDB::add_property("ResourceTile",
                          BaseResource::get_kind_prop_info("resource_kind"),
                          "set_resource_kind", "get_resource_kind");

    ClassDB::add_property("ResourceTile",
                          PropertyInfo(Variant::INT, "size", PROPERTY_HINT_ENUM,
                                       "Small,Medium,Large"),
                          "set_size", "get_size");

    ClassDB::add_signal(
        "ResourceTile",
        MethodInfo(SAmountChanged, PropertyInfo(Variant::INT, "amount")));

    ClassDB::add_signal("ResourceTile",
                        MethodInfo(SButtonStateChanged,
                                   PropertyInfo(Variant::BOOL, "enabled")));
    ClassDB::add_signal(
        "ResourceTile",
        MethodInfo(SButtonClicked,
                   PropertyInfo(Variant::STRING_NAME, "resource_name")));

    BIND_ENUM_CONSTANT(RESOURCE_TILE_SMALL);
    BIND_ENUM_CONSTANT(RESOURCE_TILE_MEDIUM);
    BIND_ENUM_CONSTANT(RESOURCE_TILE_LARGE);
}
