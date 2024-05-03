#ifndef CL_TRADING_ENTRYABLE_H_
#define CL_TRADING_ENTRYABLE_H_

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/utils.h"

namespace godot::CL {

enum EntryableKind { ENTRYABLE_CITY, ENTRYABLE_RESOURCE };
enum TileEntryType { TILE_ENTRY_ONSHORE = 1, TILE_ENTRY_OFFSHORE = 2 };

class Entryable : public Area2D {
    GDCLASS(Entryable, Area2D)
   private:
    EntryableKind entryable_kind_;
    Vector2i tile_size_;
    CollisionShape2D *col_shape_;
    Array onshore_entries_;
    Array offshore_entries_;
    Timer *restock_timer_;

   protected:
    bool button_enabled_;

    static void _bind_methods();

    void r_assign_required_components_(const float restock_time);
    void e_assign_required_components_();

    virtual void on_restock_timeout_(){};

   public:
    Entryable();
    Entryable(EntryableKind kind);
    ~Entryable();

    const static char *SButtonClicked;
    const static char *SButtonStateChanged;

    inline void start_restock_timer() {
        if (Utils::is_in_editor() || restock_timer_ == nullptr) {
            return;
        }
        restock_timer_->start();
    }

    // TODO
    inline Vector2i calculate_size_in_tiles() { return Vector2i(0, 0); }

    Dictionary get_entry_tile(const TileEntryType type) const;
    void add_entry_point(const Vector2i coords, const TileEntryType type);
    inline Vector2i get_tile_size() const { return tile_size_; }
    inline Array get_onshore_entries() const { return onshore_entries_; }
    inline Array get_offshore_entries() const { return offshore_entries_; }
    inline EntryableKind get_kind() const { return entryable_kind_; }

    inline bool get_button_enabled() const { return button_enabled_; }
    inline void set_button_enabled(const bool e) {
        button_enabled_ = e;
        emit_signal(SButtonStateChanged, e);
    }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::TileEntryType);
VARIANT_ENUM_CAST(godot::CL::EntryableKind);

#endif  // CL_TRADING_ENTRYABLE_H_
