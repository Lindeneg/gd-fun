#include "utils.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/callable.hpp>

bool godot::CL::Utils::is_in_editor() {
    return godot::Engine::get_singleton()->is_editor_hint();
}

bool godot::CL::Utils::is_in_game() { return !(is_in_editor()); }

const char *godot::CL::Utils::convert_gd_string(const Node *n) {
    ERR_FAIL_NULL_V_MSG(n, "", "cannot get_name on node that is nullptr");
    return convert_gd_string(n->get_name());
}

const char *godot::CL::Utils::convert_gd_string(StringName s) {
    return convert_gd_string(String(s));
}

const char *godot::CL::Utils::convert_gd_string(String s) {
    return s.utf8().get_data();
}

void godot::CL::Utils::connect(Node *node, const StringName name,
                               const Callable &callable) {
    ERR_FAIL_NULL_MSG(node, vformat("cannot connect to signal %s", name));
    if (!node->is_connected(name, callable)) {
        if (node->connect(name, callable) != OK) {
            WARN_PRINT_ED(vformat("%s failed to connect to signal %s",
                                  node->get_name(), name));
        }
    }
}

void godot::CL::Utils::disconnect(Node *node, const StringName name,
                                  const Callable &callable) {
    if (node == nullptr) return;
    if (node->is_connected(name, callable)) {
        node->disconnect(name, callable);
    }
}

void godot::CL::Utils::queue_delete(Node *node) {
    if (node == nullptr || node->is_queued_for_deletion()) {
        return;
    }
    node->queue_free();
}

void godot::CL::Utils::_bind_methods() {
    BIND_ENUM_CONSTANT(TILE_SURFACE_NONE);
    BIND_ENUM_CONSTANT(TILE_SURFACE_GROUND);
    BIND_ENUM_CONSTANT(TILE_SURFACE_WATER);
    BIND_ENUM_CONSTANT(TILE_SURFACE_OBSTACLE);
    BIND_ENUM_CONSTANT(TILE_SURFACE_BRIDGE);

    BIND_ENUM_CONSTANT(COLLISION_LAYER_VEHICLE);
    BIND_ENUM_CONSTANT(COLLISION_LAYER_CITY);
}
