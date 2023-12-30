#ifndef CL_TRADING_UTILS_H_
#define CL_TRADING_UTILS_H_

#include <godot_cpp/classes/engine.hpp>

namespace CL {
inline bool is_in_editor() {
    return godot::Engine::get_singleton()->is_editor_hint();
}
}  // namespace CL

#endif  // CL_TRADING_UTILS_H_
