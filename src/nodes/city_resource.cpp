#include "city_resource.h"

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
