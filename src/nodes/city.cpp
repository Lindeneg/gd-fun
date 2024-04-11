#include "city.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "../core/utils.h"

godot::CL::CityResource::CityResource()
    : resource_kind_(RESOURCE_PASSENGER), capacity_(0), amount_(0) {}

godot::CL::CityResource::~CityResource() {}

void godot::CL::CityResource::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_resource_kind"),
                         &CityResource::get_resource_kind);
    ClassDB::bind_method(D_METHOD("set_resource_kind"),
                         &CityResource::set_resource_kind);
    ClassDB::bind_method(D_METHOD("get_capacity"), &CityResource::get_capacity);
    ClassDB::bind_method(D_METHOD("set_capacity"), &CityResource::set_capacity);
    ClassDB::bind_method(D_METHOD("get_amount"), &CityResource::get_amount);
    ClassDB::bind_method(D_METHOD("set_amount"), &CityResource::set_amount);

    ClassDB::add_property("CityResource",
                          BaseResource::get_kind_prop_info("resource_kind"),
                          "set_resource_kind", "get_resource_kind");
    ClassDB::add_property("CityResource",
                          PropertyInfo(Variant::INT, "capacity"),
                          "set_capacity", "get_capacity");
    ClassDB::add_property("CityResource", PropertyInfo(Variant::INT, "amount"),
                          "set_amount", "get_amount");
}

godot::CL::City::City()
    : Entryable(),
      size_(CITY_SIZE_VILLAGE),
      supplies_(TypedArray<CityResource>{}),
      demands_(TypedArray<CityResource>{}) {}

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
    ClassDB::bind_method(D_METHOD("get_supplies"), &City::get_supplies);
    ClassDB::bind_method(D_METHOD("set_supplies", "s"), &City::set_supplies);
    ClassDB::bind_method(D_METHOD("get_demands"), &City::get_demands);
    ClassDB::bind_method(D_METHOD("set_demands", "d"), &City::set_demands);

    ClassDB::add_property(
        "City",
        PropertyInfo(Variant::INT, "size", PROPERTY_HINT_ENUM,
                     "Village,Town,Urban,Regiopolis,Metropolis"),
        "set_size", "get_size");

    ClassDB::add_property("City", PropertyInfo(Variant::ARRAY, "supplies"),
                          "set_supplies", "get_supplies");
    ClassDB::add_property("City", PropertyInfo(Variant::ARRAY, "demands"),
                          "set_demands", "get_demands");

    // BIND ENUMS
    BIND_ENUM_CONSTANT(CITY_SIZE_VILLAGE);
    BIND_ENUM_CONSTANT(CITY_SIZE_TOWN);
    BIND_ENUM_CONSTANT(CITY_SIZE_URBAN);
    BIND_ENUM_CONSTANT(CITY_SIZE_REGIO);
    BIND_ENUM_CONSTANT(CITY_SIZE_METRO);
}
