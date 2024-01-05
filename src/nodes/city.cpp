#include "city.h"

#include <godot_cpp/core/class_db.hpp>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/node_path.hpp"
#include "godot_cpp/variant/variant.hpp"

godot::CL::City::City()
    : supply_(Array()),
      demand_(Array()),
      industries_(Array()),
      size_(CITY_SIZE_VILLAGE),
      max_route_capacity_(0),
      current_route_size_(0) {}

godot::CL::City::~City() {}

void godot::CL::City::_ready() {
    set_y_sort_enabled(true);
    set_z_index(5);
}

void godot::CL::City::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_size"), &City::get_size);
    ClassDB::bind_method(D_METHOD("set_size", "s"), &City::set_size);
    ClassDB::bind_method(D_METHOD("get_onshore_entries"),
                         &City::get_onshore_entries);

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
