#include "utils.h"

#include <godot_cpp/classes/engine.hpp>

bool godot::CL::Utils::is_in_editor() {
    return godot::Engine::get_singleton()->is_editor_hint();
}

bool godot::CL::Utils::is_in_game() { return !(is_in_editor()); }

const char* godot::CL::Utils::convert_gd_string(const Node* n) {
    return convert_gd_string(n->get_name());
}

const char* godot::CL::Utils::convert_gd_string(StringName s) {
    return convert_gd_string(String(s));
}

const char* godot::CL::Utils::convert_gd_string(String s) {
    return s.utf8().get_data();
}

void godot::CL::Utils::_bind_methods() {}
