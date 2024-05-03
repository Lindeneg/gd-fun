#ifndef CL_TRADING_RESOURCE_H_
#define CL_TRADING_RESOURCE_H_

#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/entryable.h"
#include "../core/utils.h"
#include "./base_resource.h"

namespace godot::CL {

enum ResourceTileSize {
    RESOURCE_TILE_SMALL,
    RESOURCE_TILE_MEDIUM,
    RESOURCE_TILE_LARGE
};

class ResourceTile : public Entryable {
    GDCLASS(ResourceTile, Entryable)

   private:
    bool debug_;
    ResourceTileSize size_;
    ResourceKind resource_kind_;
    int capacity_amount_;
    int current_amount_;

    inline int get_restock_timeout() const {
        switch (size_) {
                //            case RESOURCE_TILE_LARGE:
                //                return 40;
                //            case RESOURCE_TILE_MEDIUM:
                //                return 45;
            default:
                return 10;
        }
    }

    inline int get_restock_amount() const {
        switch (size_) {
                //            case RESOURCE_TILE_LARGE:
                //                return 3;
                //            case RESOURCE_TILE_MEDIUM:
                //                return 2;
            default:
                return 1;
        }
    }

   protected:
    static void _bind_methods();

    void on_restock_timeout_() override;

   public:
    ResourceTile();
    ~ResourceTile();

    const static char *SAmountChanged;

    DEBUG_METHODS()

    void _ready() override;

    inline ResourceTileSize get_size() const { return size_; }
    inline ResourceKind get_resource_kind() const { return resource_kind_; }
    inline int get_capacity_amount() const { return capacity_amount_; }
    inline int get_current_amount() const { return current_amount_; }

    inline void set_size(const ResourceTileSize s) { size_ = s; }
    inline void set_resource_kind(const ResourceKind k) { resource_kind_ = k; }
    inline void set_capacity_amount(const int c) { capacity_amount_ = c; }
    inline void set_current_amount(const int c) {
        current_amount_ = c;
        emit_signal(SAmountChanged, current_amount_);
    }
};
}  // namespace godot::CL
   //
VARIANT_ENUM_CAST(godot::CL::ResourceTileSize);

#endif  // CL_TRADING_RESOURCE_H_
