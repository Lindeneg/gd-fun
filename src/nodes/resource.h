#ifndef CL_TRADING_RESOURCE_H_
#define CL_TRADING_RESOURCE_H_

#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/entryable.h"
#include "./base_resource.h"

namespace godot::CL {

class ResourceTile : public Entryable {
    GDCLASS(ResourceTile, Entryable)

   private:
    ResourceKind resource_kind_;
    int capacity_amount_;
    int current_amount_;

   protected:
    static void _bind_methods();

   public:
    ResourceTile();
    ~ResourceTile();

    const static char *SAmountChanged;

    void _ready() override;

    inline ResourceKind get_resource_kind() const { return resource_kind_; }
    inline int get_capacity_amount() const { return capacity_amount_; }
    inline int get_current_amount() const { return current_amount_; }

    inline void set_resource_kind(const ResourceKind k) { resource_kind_ = k; }
    inline void set_capacity_amount(const int c) { capacity_amount_ = c; }
    inline void set_current_amount(const int c) {
        current_amount_ = c;
        emit_signal(SAmountChanged, current_amount_);
    }
};
}  // namespace godot::CL

#endif  // CL_TRADING_RESOURCE_H_
