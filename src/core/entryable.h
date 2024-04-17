#ifndef CL_TRADING_ENTRYABLE_H_
#define CL_TRADING_ENTRYABLE_H_

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot::CL {

enum TileEntryType { TILE_ENTRY_ONSHORE = 1, TILE_ENTRY_OFFSHORE = 2 };

class Entryable : public Area2D {
    GDCLASS(Entryable, Area2D)
   private:
    Vector2i tile_size_;
    CollisionShape2D *col_shape_;
    Array onshore_entries_;
    Array offshore_entries_;

   protected:
    bool button_enabled_;

    static void _bind_methods();

    void r_assign_required_components_();
    void e_assign_required_components_();

   public:
    Entryable();
    ~Entryable();

    const static char *SButtonClicked;
    const static char *SButtonStateChanged;

    inline Vector2i calculate_size_in_tiles() { return Vector2i(0, 0); }

    Dictionary get_entry_tile(const TileEntryType type) const;
    void add_entry_point(const Vector2i coords, const TileEntryType type);
    inline Vector2i get_tile_size() const { return tile_size_; }
    inline Array get_onshore_entries() const { return onshore_entries_; }
    inline Array get_offshore_entries() const { return offshore_entries_; }
    inline bool get_button_enabled() const { return button_enabled_; }
    inline void set_button_enabled(const bool e) {
        button_enabled_ = e;
        emit_signal(SButtonStateChanged, e);
    }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::TileEntryType);

#endif  // CL_TRADING_ENTRYABLE_H_
