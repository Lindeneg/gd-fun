#ifndef CL_TRADING_UTILS_H_
#define CL_TRADING_UTILS_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace godot::CL {
class Utils : public Object {
    GDCLASS(Utils, Object)

   protected:
    static void _bind_methods();

   public:
    static bool is_in_editor();
    static bool is_in_game();
    static const char* convert_gd_string(const Node* n);
    static const char* convert_gd_string(String s);
    static const char* convert_gd_string(StringName s);
};
}  // namespace godot::CL

#endif  // CL_TRADING_UTILS_H_
