#ifndef CL_TRADING_CITY_RESOURCE_H_
#define CL_TRADING_CITY_RESOURCE_H_

#include <godot_cpp/classes/resource.hpp>

#include "./base_resource.h"

namespace godot::CL {
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
}  // namespace godot::CL
#endif  // CL_TRADING_CITY_RESOURCE_H_
