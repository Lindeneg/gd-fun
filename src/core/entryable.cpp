#include "entryable.h"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../core/utils.h"

const char *godot::CL::Entryable::SButtonClicked{"btn_clicked"};
const char *godot::CL::Entryable::SButtonStateChanged{"btn_state_changed"};

godot::CL::Entryable::Entryable() : Entryable(ENTRYABLE_CITY) {}

godot::CL::Entryable::Entryable(EntryableKind kind)
    : entryable_kind_(kind),
      tile_size_(Vector2i(0, 0)),
      col_shape_(nullptr),
      onshore_entries_(Array()),
      offshore_entries_(Array()) {}

godot::CL::Entryable::~Entryable() {
#ifdef CL_TRADING_DEBUG
    printf("Entryable: queuing shape for deletion\n");
#endif
    Utils::queue_delete(col_shape_);
    col_shape_ = nullptr;
}

void godot::CL::Entryable::add_entry_point(const Vector2i coords,
                                           const TileEntryType type) {
    if (type == TILE_ENTRY_ONSHORE) {
        onshore_entries_.append(coords);
    }
    if (type == TILE_ENTRY_OFFSHORE) {
        offshore_entries_.append(coords);
    }
}

void godot::CL::Entryable::r_assign_required_components_() {
    if (col_shape_ == nullptr) {
        Node *col{find_child("*CollisionShape*")};
        ERR_FAIL_NULL_MSG(col, "required component CollisionShape missing");
        col_shape_ = static_cast<CollisionShape2D *>(col);
    }
}

void godot::CL::Entryable::e_assign_required_components_() {
    ERR_FAIL_COND_EDMSG(col_shape_ != nullptr,
                        "component CollisionShape already assigned");
    Node *col{find_child("*CollisionShape*")};
    if (col == nullptr) {
        col_shape_ = Utils::create_component<CollisionShape2D>(this);
        auto shape = memnew(CircleShape2D);
        shape->set_local_to_scene(true);
        col_shape_->set_shape(shape);
    } else {
        col_shape_ = static_cast<CollisionShape2D *>(col);
    }
}

godot::Dictionary godot::CL::Entryable::get_entry_tile(
    const TileEntryType type) const {
    Dictionary result{};
    result["coords"] = Vector2i();
    result["found"] = false;
    if (type == TILE_ENTRY_ONSHORE && onshore_entries_.size() > 0) {
        result["coords"] = onshore_entries_[0];
        result["found"] = true;
        return result;
    }
    if (type == TILE_ENTRY_OFFSHORE && offshore_entries_.size() > 0) {
        result["coords"] = offshore_entries_[0];
        result["found"] = true;
        return result;
    }
    return result;
}

void godot::CL::Entryable::_bind_methods() {
    // BIND METHODS
    ClassDB::bind_method(D_METHOD("get_kind"), &Entryable::get_kind);
    ClassDB::bind_method(D_METHOD("get_onshore_entries"),
                         &Entryable::get_onshore_entries);
    ClassDB::bind_method(D_METHOD("get_offshore_entries"),
                         &Entryable::get_offshore_entries);
    ClassDB::bind_method(D_METHOD("get_entry_tile", "t"),
                         &Entryable::get_entry_tile);
    ClassDB::bind_method(D_METHOD("get_button_enabled"),
                         &Entryable::get_button_enabled);
    ClassDB::bind_method(D_METHOD("set_button_enabled", "e"),
                         &Entryable::set_button_enabled);

    // BIND ENUMS
    BIND_ENUM_CONSTANT(TILE_ENTRY_ONSHORE);
    BIND_ENUM_CONSTANT(TILE_ENTRY_OFFSHORE);

    BIND_ENUM_CONSTANT(ENTRYABLE_CITY);
    BIND_ENUM_CONSTANT(ENTRYABLE_RESOURCE);
}
