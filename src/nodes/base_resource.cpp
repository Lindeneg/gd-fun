#include "base_resource.h"

godot::CL::BaseResource::BaseResource()
    : kind_(RESOURCE_PASSENGER),
      value_(0),
      weight_(0),
      value_history_(Array{}) {}

godot::CL::BaseResource::~BaseResource() {}

void godot::CL::BaseResource::_ready() {
    value_history_.clear();
    value_history_.push_back(value_);
}

void godot::CL::BaseResource::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_kind"), &BaseResource::get_kind);
    ClassDB::bind_method(D_METHOD("set_kind", "r"), &BaseResource::set_kind);
    ClassDB::bind_method(D_METHOD("get_value"), &BaseResource::get_value);
    ClassDB::bind_method(D_METHOD("set_value", "v"), &BaseResource::set_value);
    ClassDB::bind_method(D_METHOD("get_weight"), &BaseResource::get_weight);
    ClassDB::bind_method(D_METHOD("set_weight", "w"),
                         &BaseResource::set_weight);
    ClassDB::bind_method(D_METHOD("get_value_history"),
                         &BaseResource::get_value_history);

    ClassDB::add_property(
        "BaseResource",
        PropertyInfo(Variant::INT, "value", PROPERTY_HINT_RANGE, "1,500"),
        "set_value", "get_value");

    ClassDB::add_property(
        "BaseResource",
        PropertyInfo(Variant::INT, "weight", PROPERTY_HINT_RANGE, "1,10"),
        "set_weight", "get_weight");

    ClassDB::add_property(
        "BaseResource",
        PropertyInfo(Variant::INT, "resource_kind", PROPERTY_HINT_ENUM,
                     "Passenger:0,Mail:1,Grain:2,Fish:3,Rock:4,Iron:5,Wood:6,"
                     "Boar:7,Pig:8,Chicken:9,Sheep:10,Poppy:11,Food:12,Beer:13,"
                     "Cobble:14,Tools:15,Textiles:16,Medicine:17"),
        "set_kind", "get_kind");

    // BIND ENUMS
    BIND_ENUM_CONSTANT(RESOURCE_PASSENGER);
    BIND_ENUM_CONSTANT(RESOURCE_MAIL);
    BIND_ENUM_CONSTANT(RESOURCE_GRAIN);
    BIND_ENUM_CONSTANT(RESOURCE_FISH);
    BIND_ENUM_CONSTANT(RESOURCE_ROCK);
    BIND_ENUM_CONSTANT(RESOURCE_IRON);
    BIND_ENUM_CONSTANT(RESOURCE_WOOD);
    BIND_ENUM_CONSTANT(RESOURCE_BOAR);
    BIND_ENUM_CONSTANT(RESOURCE_PIG);
    BIND_ENUM_CONSTANT(RESOURCE_CHICKEN);
    BIND_ENUM_CONSTANT(RESOURCE_SHEEP);
    BIND_ENUM_CONSTANT(RESOURCE_POPPY);
    BIND_ENUM_CONSTANT(RESOURCE_FOOD);
    BIND_ENUM_CONSTANT(RESOURCE_BEER);
    BIND_ENUM_CONSTANT(RESOURCE_COBBLE);
    BIND_ENUM_CONSTANT(RESOURCE_TOOLS);
    BIND_ENUM_CONSTANT(RESOURCE_TEXTILES);
    BIND_ENUM_CONSTANT(RESOURCE_MEDICINE);
}
