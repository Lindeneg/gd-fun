#ifndef CL_TRADING_INDUSTRY_H_
#define CL_TRADING_INDUSTRY_H_

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/timer.hpp>

#include "../core/utils.h"
#include "./base_resource.h"

namespace godot::CL {

enum IndustryState { INDUSTRY_STATE_IDLE, INDUSTRY_STATE_PROCESSING };

class Industry : public Resource {
    GDCLASS(Industry, Resource)

   private:
    bool debug_;
    int total_profits_;
    Timer *timer_;
    StringName owner_;
    IndustryState state_;
    ResourceKind in_kind_;
    ResourceKind out_kind_;
    float time_to_convert_;
    int conversion_amount_;
    int in_amount_;
    int out_amount_;

    void on_process_resource_finished_();
    void queue_process_resource_();

   protected:
    static void _bind_methods();

   public:
    Industry();
    ~Industry();

    DEBUG_METHODS()

    const static char *SResourceProcessStarted;
    const static char *SResourceProcessFinished;

    Timer *initialize_timer();
    inline Timer *get_timer() const { return timer_; }

    inline bool has_timer() const { return timer_ != nullptr; }
    inline void add_to_profits(const int n) { total_profits_ += n; }

    inline StringName get_owner() const { return owner_; }
    inline ResourceKind get_in_kind() const { return in_kind_; }
    inline ResourceKind get_out_kind() const { return out_kind_; }
    inline float get_time_to_convert() const { return time_to_convert_; }
    inline int get_conversion_amount() const { return conversion_amount_; }
    inline int get_in_amount() const { return in_amount_; }
    inline int get_out_amount() const { return out_amount_; }
    inline IndustryState get_state() const { return state_; }
    inline int get_total_profits() const { return total_profits_; }

    inline void set_owner(const StringName owner) { owner_ = owner; }
    inline void set_in_kind(const ResourceKind k) { in_kind_ = k; }
    inline void set_total_profits(const int a) { total_profits_ = a; }
    inline void set_out_kind(const ResourceKind k) { out_kind_ = k; }
    inline void set_time_to_convert(const float t) { time_to_convert_ = t; }
    inline void set_conversion_amount(const int c) { conversion_amount_ = c; }
    inline void set_in_amount(const int a) {
        in_amount_ = a;
        queue_process_resource_();
    }
    inline void set_out_amount(const int a) { out_amount_ = a; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::IndustryState);

#endif  // CL_TRADING_INDUSTRY_H_
