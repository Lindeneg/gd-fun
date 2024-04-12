#include "industry.h"

godot::CL::Industry::Industry()
    : in_kind_(RESOURCE_PASSENGER),
      out_kind_(RESOURCE_PASSENGER),
      capacity_(0),
      amount_(0) {}

godot::CL::Industry::~Industry() {}

void godot::CL::Industry::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_in_kind"), &Industry::get_in_kind);
    ClassDB::bind_method(D_METHOD("get_out_kind"), &Industry::get_out_kind);
    ClassDB::bind_method(D_METHOD("set_in_kind"), &Industry::set_in_kind);
    ClassDB::bind_method(D_METHOD("set_out_kind"), &Industry::set_out_kind);
    ClassDB::bind_method(D_METHOD("get_capacity"), &Industry::get_capacity);
    ClassDB::bind_method(D_METHOD("set_capacity"), &Industry::set_capacity);
    ClassDB::bind_method(D_METHOD("get_amount"), &Industry::get_amount);
    ClassDB::bind_method(D_METHOD("set_amount"), &Industry::set_amount);

    ClassDB::add_property("Industry", BaseResource::get_kind_prop_info("in"),
                          "set_in_kind", "get_in_kind");
    ClassDB::add_property("Industry", BaseResource::get_kind_prop_info("out"),
                          "set_out_kind", "get_out_kind");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "capacity"),
                          "set_capacity", "get_capacity");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "amount"),
                          "set_amount", "get_amount");
}
