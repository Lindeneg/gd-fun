#ifndef CL_TRADING_UTILS_H_
#define CL_TRADING_UTILS_H_

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/wrapped.hpp"
namespace godot::CL {
class Utils : public Object {
    GDCLASS(Utils, Object)

   protected:
    static void _bind_methods();

   public:
    static bool is_in_editor();
    static bool is_in_game();
};
}  // namespace godot::CL

#endif  // CL_TRADING_UTILS_H_
