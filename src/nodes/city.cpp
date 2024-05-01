#include "city.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "../core/utils.h"
#include "./industry.h"

namespace godot::CL {
static bool debug{false};
void CITYLOG NEW_LOG(City)
}  // namespace godot::CL

// SIGNALS
const char *godot::CL::City::SSuppliesChanged{"supplies_changed"};
const char *godot::CL::City::SSupplyChanged{"supply_changed"};
const char *godot::CL::City::SDemandsChanged{"demands_changed"};
const char *godot::CL::City::SIndustriesChanged{"industries_changed"};

godot::CL::City::City()
    : Entryable(ENTRYABLE_CITY),
      size_(CITY_SIZE_VILLAGE),
      supplies_(TypedArray<CityResource>{}),
      demands_(TypedArray<CityResource>{}),
      industries_(TypedArray<Industry>{}),
      y_container_offset_(0.0f),
      y_button_offset_(0.0f) {}

godot::CL::City::~City() {}

int godot::CL::City::consume_resource(ResourceKind kind, int amount) {
    for (int i = 0; i < supplies_.size(); i++) {
        CityResource *supply{cast_to<CityResource>(supplies_[i])};
        if (supply->get_resource_kind() == kind) {
            auto s_amount{supply->get_amount()};
            if (s_amount == 0) return 0;
            auto actual_amount{amount};
            if (amount > s_amount) {
                actual_amount = s_amount;
            }
            int diff{s_amount - actual_amount};
            supply->set_amount(diff);
#ifdef CL_TRADING_CITY_DEBUG
            CITYLOG(this, "has %d left of resource %d after consumption\n",
                    diff, kind);
#endif
            emit_signal(SSupplyChanged, kind, diff);
            return actual_amount;
        }
    }
    for (int i = 0; i < industries_.size(); i++) {
        Industry *industry{cast_to<Industry>(industries_[i])};
        if (industry->get_out_kind() == kind) {
            int i_amount{industry->get_out_amount()};
            if (i_amount == 0) return 0;
            int actual_amount{amount};
            if (amount > i_amount) {
                actual_amount = i_amount;
            }
            int diff{i_amount - actual_amount};
            industry->set_out_amount(diff);
#ifdef CL_TRADING_CITY_DEBUG
            CITYLOG(this,
                    "has %d left of industry resource %d after consumption\n",
                    diff, kind);
#endif
            emit_signal(SSupplyChanged, kind, diff);
            return actual_amount;
        }
    }
    return 0;
}

godot::CL::CityReceiveResult godot::CL::City::receive_resource(
    ResourceKind kind, int amount) {
    CityReceiveResult result{0, amount};
    for (int i = 0; i < demands_.size(); i++) {
        CityResource *demand{cast_to<CityResource>(demands_[i])};
        if (demand->get_resource_kind() == kind) {
#ifdef CL_TRADING_CITY_DEBUG
            CITYLOG(this, "has received %d of resource %d\n", amount, kind);
#endif
            result.accepted_amount = amount;
            return result;
        }
    }
    for (int i = 0; i < industries_.size(); i++) {
        Industry *industry{cast_to<Industry>(industries_[i])};
        if (industry->get_in_kind() == kind) {
            int actual_amount{industry->get_in_amount() + amount};
            industry->set_in_amount(actual_amount);
#ifdef CL_TRADING_CITY_DEBUG
            CITYLOG(this, "has received %d of industry resource %d, total %d\n",
                    amount, kind, actual_amount);
#endif
            result.accepted_amount = amount;
            return result;
        }
    }
    return result;
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
    ClassDB::bind_method(D_METHOD("get_supplies"), &City::get_supplies);
    ClassDB::bind_method(D_METHOD("set_supplies", "s"), &City::set_supplies);
    ClassDB::bind_method(D_METHOD("get_demands"), &City::get_demands);
    ClassDB::bind_method(D_METHOD("set_demands", "d"), &City::set_demands);
    ClassDB::bind_method(D_METHOD("get_industries"), &City::get_industries);
    ClassDB::bind_method(D_METHOD("set_industries", "d"),
                         &City::set_industries);
    ClassDB::bind_method(D_METHOD("get_y_container_offset"),
                         &City::get_y_container_offset);
    ClassDB::bind_method(D_METHOD("set_y_container_offset", "o"),
                         &City::set_y_container_offset);
    ClassDB::bind_method(D_METHOD("get_y_btn_offset"), &City::get_y_btn_offset);
    ClassDB::bind_method(D_METHOD("set_y_btn_offset", "o"),
                         &City::set_y_btn_offset);

    ClassDB::add_property(
        "City",
        PropertyInfo(Variant::INT, "size", PROPERTY_HINT_ENUM,
                     "Village,Town,Urban,Regiopolis,Metropolis"),
        "set_size", "get_size");

    ClassDB::add_property("City",
                          PropertyInfo(Variant::FLOAT, "y_container_offset"),
                          "set_y_container_offset", "get_y_container_offset");
    ClassDB::add_property("City", PropertyInfo(Variant::FLOAT, "y_btn_offset"),
                          "set_y_btn_offset", "get_y_btn_offset");

    ClassDB::add_property(
        "City",
        PropertyInfo(Variant::ARRAY, "supplies", PROPERTY_HINT_ARRAY_TYPE,
                     MAKE_RESOURCE_TYPE_HINT("CityResource")),
        "set_supplies", "get_supplies");

    ClassDB::add_property(
        "City",
        PropertyInfo(Variant::ARRAY, "demands", PROPERTY_HINT_ARRAY_TYPE,
                     MAKE_RESOURCE_TYPE_HINT("CityResource")),
        "set_demands", "get_demands");

    ClassDB::add_property(
        "City",
        PropertyInfo(Variant::ARRAY, "industries", PROPERTY_HINT_ARRAY_TYPE,
                     MAKE_RESOURCE_TYPE_HINT("Industry")),
        "set_industries", "get_industries");

    // BIND ENUMS
    BIND_ENUM_CONSTANT(CITY_SIZE_VILLAGE);
    BIND_ENUM_CONSTANT(CITY_SIZE_TOWN);
    BIND_ENUM_CONSTANT(CITY_SIZE_URBAN);
    BIND_ENUM_CONSTANT(CITY_SIZE_REGIO);
    BIND_ENUM_CONSTANT(CITY_SIZE_METRO);

    // SIGNALS
    ClassDB::add_signal(
        "City",
        MethodInfo(SSuppliesChanged, PropertyInfo(Variant::ARRAY, "supplies")));
    ClassDB::add_signal(
        "City",
        MethodInfo(SDemandsChanged, PropertyInfo(Variant::ARRAY, "demands")));
    ClassDB::add_signal("City",
                        MethodInfo(SIndustriesChanged,
                                   PropertyInfo(Variant::ARRAY, "industries")));
    ClassDB::add_signal("City",
                        MethodInfo(SButtonStateChanged,
                                   PropertyInfo(Variant::BOOL, "enabled")));
    ClassDB::add_signal(
        "City", MethodInfo(SButtonClicked,
                           PropertyInfo(Variant::STRING_NAME, "city_name")));
    ClassDB::add_signal(
        "City", MethodInfo(SSupplyChanged, PropertyInfo(Variant::INT, "kind"),
                           PropertyInfo(Variant::INT, "new_amount")));
}
