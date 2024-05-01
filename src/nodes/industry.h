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
    float time_to_convert_;
    int conversion_amount_;
    int in_amount_;
    int out_amount_;

   protected:
    static void _bind_methods();

   public:
    Industry();
    ~Industry();

    inline ResourceKind get_in_kind() const { return in_kind_; }
    inline ResourceKind get_out_kind() const { return out_kind_; }
    inline float get_time_to_convert() const { return time_to_convert_; }
    inline int get_conversion_amount() const { return conversion_amount_; }
    inline int get_in_amount() const { return in_amount_; }
    inline int get_out_amount() const { return out_amount_; }

    inline void set_in_kind(const ResourceKind k) { in_kind_ = k; }
    inline void set_out_kind(const ResourceKind k) { out_kind_ = k; }
    inline void set_time_to_convert(const float t) { time_to_convert_ = t; }
    inline void set_conversion_amount(const int c) { conversion_amount_ = c; }
    inline void set_in_amount(const int a) { in_amount_ = a; }
    inline void set_out_amount(const int a) { out_amount_ = a; }
};
}  // namespace godot::CL

#endif  // CL_TRADING_INDUSTRY_H_
