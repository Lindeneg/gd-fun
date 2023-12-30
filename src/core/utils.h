#include <godot_cpp/classes/engine.hpp>

namespace CL {
inline bool is_in_editor() {
    return godot::Engine::get_singleton()->is_editor_hint();
}
}  // namespace CL
