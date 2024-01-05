#include "utils.h"

#include <godot_cpp/classes/engine.hpp>

bool godot::CL::Utils::is_in_editor() {
    return godot::Engine::get_singleton()->is_editor_hint();
}

bool godot::CL::Utils::is_in_game() { return !(is_in_editor()); }

void godot::CL::Utils::_bind_methods() {}
