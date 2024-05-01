#include "industry.h"

godot::CL::Industry::Industry()
    : in_kind_(RESOURCE_PASSENGER),
      out_kind_(RESOURCE_PASSENGER),
      time_to_convert_(0.0f),
      conversion_amount_(0),
      in_amount_(0),
      out_amount_(0) {}

godot::CL::Industry::~Industry() {}

void godot::CL::Industry::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_in_kind"), &Industry::get_in_kind);
    ClassDB::bind_method(D_METHOD("get_out_kind"), &Industry::get_out_kind);
    ClassDB::bind_method(D_METHOD("get_conversion_amount"),
                         &Industry::get_conversion_amount);
    ClassDB::bind_method(D_METHOD("set_conversion_amount", "c"),
                         &Industry::set_conversion_amount);
    ClassDB::bind_method(D_METHOD("set_in_kind", "k"), &Industry::set_in_kind);
    ClassDB::bind_method(D_METHOD("set_out_kind", "k"),
                         &Industry::set_out_kind);

    ClassDB::bind_method(D_METHOD("get_in_amount"), &Industry::get_in_amount);
    ClassDB::bind_method(D_METHOD("set_in_amount", "a"),
                         &Industry::set_in_amount);
    ClassDB::bind_method(D_METHOD("get_out_amount"), &Industry::get_out_amount);
    ClassDB::bind_method(D_METHOD("set_out_amount", "a"),
                         &Industry::set_out_amount);

    ClassDB::bind_method(D_METHOD("get_time_to_convert"),
                         &Industry::get_time_to_convert);
    ClassDB::bind_method(D_METHOD("set_time_to_convert", "t"),
                         &Industry::set_time_to_convert);

    ClassDB::add_property("Industry", BaseResource::get_kind_prop_info("in"),
                          "set_in_kind", "get_in_kind");
    ClassDB::add_property("Industry", BaseResource::get_kind_prop_info("out"),
                          "set_out_kind", "get_out_kind");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "in_amount"),
                          "set_in_amount", "get_in_amount");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "out_amount"),
                          "set_out_amount", "get_out_amount");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "conversion"),
                          "set_conversion_amount", "get_conversion_amount");
    ClassDB::add_property("Industry",
                          PropertyInfo(Variant::FLOAT, "time_to_convert"),
                          "set_time_to_convert", "get_time_to_convert");
}
