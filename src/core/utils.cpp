#include "utils.h"

#include <godot_cpp/classes/engine.hpp>

bool CL::is_in_editor() {
    return godot::Engine::get_singleton()->is_editor_hint();
}
