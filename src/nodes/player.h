#ifndef CL_TRADING_PLAYER_H_
#define CL_TRADING_PLAYER_H_

#include <godot_cpp/classes/node.hpp>

namespace godot::CL {

enum PlayerLevel {
    PLAYER_LEVEL_NOVICE,
    PLAYER_LEVEL_INTERMEDIATE,
    PLAYER_LEVEL_ADVANCED,
    PLAYER_LEVEL_MASTER
};

class Player : public Node {
    GDCLASS(Player, Node)

   private:
    StringName start_city_;
    Array connections_;
    PlayerLevel level_;
    int gold_;
    int range_;
    bool is_active_;

   protected:
    static void _bind_methods();

   public:
    Player();
    ~Player();

    void _ready() override;

    inline StringName get_start_city() const { return start_city_; }
    inline Array get_connections() const { return connections_; }
    inline PlayerLevel get_player_level() const { return level_; }
    inline int get_gold() const { return gold_; }
    inline int get_range() const { return range_; }
    inline bool get_is_active() const { return is_active_; }

    inline bool has_connection(const StringName n) {
        return connections_.has(n);
    }
    inline void set_connections(const Array c) { connections_ = c; }
    inline void add_connection(const StringName n) {
        if (!connections_.has(n)) {
            connections_.append(n);
            emit_signal("connection_added", n);
        }
    }
    inline void remove_connection(const StringName n) {
        if (connections_.has(n)) {
            connections_.erase(n);
            emit_signal("connection_removed", n);
        }
    }
    inline void set_start_city(const StringName s) { start_city_ = s; }
    inline void set_player_level(const PlayerLevel l) { level_ = l; }
    inline void set_gold(const int g) { gold_ = g; }
    inline void set_range(const int r) { range_ = r; }
    inline void set_is_active(const bool a) { is_active_ = a; }
};
}  // namespace godot::CL
   //
VARIANT_ENUM_CAST(godot::CL::PlayerLevel);

#endif  // CL_TRADING_PLAYER_H_
