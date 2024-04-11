#ifndef CL_TRADING_CITY_H_
#define CL_TRADING_CITY_H_

#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/entryable.h"
#include "./base_resource.h"

namespace godot::CL {

enum CitySize {
    CITY_SIZE_VILLAGE,
    CITY_SIZE_TOWN,
    CITY_SIZE_URBAN,
    CITY_SIZE_REGIO,
    CITY_SIZE_METRO
};

class CityResource : public Resource {
    GDCLASS(CityResource, Resource)

   private:
    ResourceKind resource_kind_;
    int capacity_;
    int amount_;

   protected:
    static void _bind_methods();

   public:
    CityResource();
    ~CityResource();


    inline ResourceKind get_resource_kind() const { return resource_kind_; }
    inline int get_capacity() const { return capacity_; }
    inline int get_amount() const { return amount_; }

    inline void set_resource_kind(const ResourceKind k) { resource_kind_ = k; }
    inline void set_capacity(const int c) { capacity_ = c; }
    inline void set_amount(const int a) { amount_ = a; }
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
    CitySize size_;
    TypedArray<CityResource> supplies_;
    TypedArray<CityResource> demands_;

   protected:
    static void _bind_methods();

   public:
    City();
    ~City();

    void _ready() override;

    inline CitySize get_size() const { return size_; }
    inline void set_size(const CitySize s) { size_ = s; }
    inline TypedArray<CityResource> get_supplies() const { return supplies_; }
    inline void set_supplies(TypedArray<CityResource> s) { supplies_ = s; }
    inline TypedArray<CityResource> get_demands() const { return demands_; }
    inline void set_demands(TypedArray<CityResource> d) { demands_ = d; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::CitySize);

#endif  // CL_TRADING_CITY_H_
