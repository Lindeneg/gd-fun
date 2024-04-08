#include "city.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "../core/utils.h"

godot::CL::City::City() : Entryable(), size_(CITY_SIZE_VILLAGE) {}

godot::CL::City::~City() {}

void godot::CL::City::_ready() {
    if (Utils::is_in_editor()) {
        e_assign_required_components_();
    } else {
        r_assign_required_components_();
    }
    set_y_sort_enabled(true);
    set_z_index(5);
    set_monitorable(false);
    set_collision_layer(COLLISION_LAYER_CITY);
    set_collision_mask(COLLISION_LAYER_VEHICLE);
}

void godot::CL::City::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_size"), &City::get_size);
    ClassDB::bind_method(D_METHOD("set_size", "s"), &City::set_size);
    ClassDB::add_property(
        "City",
        PropertyInfo(Variant::INT, "size", PROPERTY_HINT_ENUM,
                     "Village,Town,Urban,Regiopolis,Metropolis"),
        "set_size", "get_size");

    // BIND ENUMS
    BIND_ENUM_CONSTANT(CITY_SIZE_VILLAGE);
    BIND_ENUM_CONSTANT(CITY_SIZE_TOWN);
    BIND_ENUM_CONSTANT(CITY_SIZE_URBAN);
    BIND_ENUM_CONSTANT(CITY_SIZE_REGIO);
    BIND_ENUM_CONSTANT(CITY_SIZE_METRO);
}
