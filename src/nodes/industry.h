#ifndef CL_TRADING_INDUSTRY_H_
#define CL_TRADING_INDUSTRY_H_

#include <godot_cpp/classes/resource.hpp>

#include "./base_resource.h"

namespace godot::CL {
class Industry : public Resource {
    GDCLASS(Industry, Resource)

   private:
    ResourceKind in_kind_;
    ResourceKind out_kind_;
    int capacity_;
    int amount_;

   protected:
    static void _bind_methods();

   public:
    Industry();
    ~Industry();

    inline ResourceKind get_in_kind() const { return in_kind_; }
    inline ResourceKind get_out_kind() const { return out_kind_; }
    inline int get_capacity() const { return capacity_; }
    inline int get_amount() const { return amount_; }

    inline void set_in_kind(const ResourceKind k) { in_kind_ = k; }
    inline void set_out_kind(const ResourceKind k) { out_kind_ = k; }
    inline void set_capacity(const int c) { capacity_ = c; }
    inline void set_amount(const int a) { amount_ = a; }
};
}  // namespace godot::CL

#endif  // CL_TRADING_INDUSTRY_H_
