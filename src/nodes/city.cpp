#include "city.h"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../core/utils.h"
#include "godot_cpp/classes/circle_shape2d.hpp"

godot::CL::City::City()
    : col_shape_(nullptr),
      //      supply_(Array()),
      //      demand_(Array()),
      // industries_(Array()),
      onshore_entries_(Array()),
      offshore_entries_(Array()),
      size_(CITY_SIZE_VILLAGE),
      max_route_capacity_(0),
      current_route_size_(0) {}

godot::CL::City::~City() {}

void godot::CL::City::add_city_entry_point(const Vector2i coords,
                                           const CityEntryType type) {
    if (type == CITY_ENTRY_ONSHORE) {
        onshore_entries_.append(coords);
    }
    if (type == CITY_ENTRY_OFFSHORE) {
        offshore_entries_.append(coords);
    }
}

void godot::CL::City::r_assign_required_components_() {
    if (col_shape_ == nullptr) {
        Node *col{find_child("*CollisionShape*")};
        ERR_FAIL_NULL_MSG(col, "required component CollisionShape missing");
        col_shape_ = static_cast<CollisionShape2D *>(col);
    }
}

void godot::CL::City::e_assign_required_components_() {
    ERR_FAIL_COND_EDMSG(col_shape_ != nullptr,
                        "component CollisionShape already assigned");
    Node *col{find_child("*CollisionShape*")};
    if (col == nullptr) {
        col_shape_ = Utils::create_component<CollisionShape2D>(this);
        auto shape = memnew(CircleShape2D);
        shape->set_local_to_scene(true);
        col_shape_->set_shape(shape);
    } else {
        col_shape_ = static_cast<CollisionShape2D *>(col);
    }
}

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
    ClassDB::bind_method(D_METHOD("get_onshore_entries"),
                         &City::get_onshore_entries);
    ClassDB::bind_method(D_METHOD("get_offshore_entries"),
                         &City::get_offshore_entries);
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

    BIND_ENUM_CONSTANT(CITY_ENTRY_ONSHORE);
    BIND_ENUM_CONSTANT(CITY_ENTRY_OFFSHORE);
}
