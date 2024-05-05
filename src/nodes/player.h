#ifndef CL_TRADING_PLAYER_H_
#define CL_TRADING_PLAYER_H_

#include <godot_cpp/classes/node.hpp>

#include "./player_finance.h"

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
    bool debug_;
    StringName start_city_;
    Array connections_;
    PlayerLevel level_;
    PlayerFinance *finance_;
    bool is_active_;

    void r_assign_required_components_();

   protected:
    static void _bind_methods();

   public:
    Player();
    ~Player();

    void _ready() override;

    DEBUG_METHODS()

    bool has_connection(const StringName n);
    void add_connection(const StringName n);
    void remove_connection(const StringName n);

    inline int get_range() const {
        switch (level_) {
            case PLAYER_LEVEL_MASTER:
                return 500;
            case PLAYER_LEVEL_ADVANCED:
                return 300;
            case PLAYER_LEVEL_INTERMEDIATE:
                return 200;
            default:
                return 100;
        }
    }

    inline PlayerFinance *get_finance() const { return finance_; }
    inline StringName get_start_city() const { return start_city_; }
    inline Array get_connections() const { return connections_; }
    inline PlayerLevel get_player_level() const { return level_; }
    inline bool get_is_active() const { return is_active_; }
    inline void set_connections(const Array c) { connections_ = c; }

    inline void set_start_city(const StringName s) { start_city_ = s; }
    inline void set_player_level(const PlayerLevel l) { level_ = l; }
    inline void set_is_active(const bool a) { is_active_ = a; }
};
}  // namespace godot::CL
   //
VARIANT_ENUM_CAST(godot::CL::PlayerLevel);

#endif  // CL_TRADING_PLAYER_H_
