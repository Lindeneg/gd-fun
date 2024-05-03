#ifndef CL_TRADING_CITY_H_
#define CL_TRADING_CITY_H_

#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/entryable.h"
#include "./city_resource.h"
#include "./industry.h"
#include "utils.h"

namespace godot::CL {

enum CitySize {
    CITY_SIZE_VILLAGE,
    CITY_SIZE_TOWN,
    CITY_SIZE_URBAN,
    CITY_SIZE_REGIO,
    CITY_SIZE_METRO
};

struct CityReceiveResult {
    StringName industry;
    bool industry_resource;
    int accepted_amount;
    int amount;
};

/* City has supply, demand and a size.
 * City also only place where
 * Industry can be located.
 *
 * City size grows over time allowing
 * for more supply, demand and Industry. */
class City : public Entryable {
    GDCLASS(City, Entryable)

   private:
    bool debug_;
    CitySize size_;
    TypedArray<CityResource> supplies_;
    TypedArray<CityResource> demands_;
    TypedArray<Industry> industries_;
    float y_container_offset_;
    float y_button_offset_;

    inline void on_resource_process_finished_(int kind, int amount) {
        emit_signal(SSupplyChanged, kind, amount);
    }

    inline int get_restock_timeout() const {
        switch (size_) {
                //            case CITY_SIZE_METRO:
                //                return 25;
                //            case CITY_SIZE_REGIO:
                //                return 30;
                //            case CITY_SIZE_URBAN:
                //                return 35;
                //            case CITY_SIZE_TOWN:
                //                return 50;
            default:
                return 10;
        }
    }

    inline int get_restock_amount() const {
        switch (size_) {
                //            case CITY_SIZE_METRO:
                //                return 3;
                //            case CITY_SIZE_REGIO:
                //                return 2;
            default:
                return 1;
        }
    }

   protected:
    static void _bind_methods();

    void on_restock_timeout_() override;

   public:
    City();
    ~City();

    const static char *SSuppliesChanged;
    const static char *SSupplyChanged;
    const static char *SDemandsChanged;
    const static char *SIndustriesChanged;

    void _ready() override;

    int consume_resource(ResourceKind kind, int amount);
    CityReceiveResult receive_resource(ResourceKind kind, int amount);

    DEBUG_METHODS()

    inline CitySize get_size() const { return size_; }
    inline float get_y_container_offset() const { return y_container_offset_; }
    inline float get_y_btn_offset() const { return y_button_offset_; }
    inline void set_size(const CitySize s) { size_ = s; }
    inline TypedArray<CityResource> get_supplies() const { return supplies_; }
    inline void set_supplies(TypedArray<CityResource> s) {
        supplies_ = s;
        emit_signal(SSuppliesChanged, s);
    }
    inline TypedArray<Industry> get_industries() const { return industries_; }
    inline void set_industries(TypedArray<Industry> i) {
        industries_ = i;
        emit_signal(SIndustriesChanged, i);
    }
    inline TypedArray<CityResource> get_demands() const { return demands_; }
    inline void set_demands(TypedArray<CityResource> d) {
        demands_ = d;
        emit_signal(SDemandsChanged, d);
    }
    inline void set_y_container_offset(const float o) {
        y_container_offset_ = o;
    }
    inline void set_y_btn_offset(const float o) { y_button_offset_ = o; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::CitySize);

#endif  // CL_TRADING_CITY_H_
