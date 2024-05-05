#ifndef CL_TRADING_PLAYER_MANAGER_H_
#define CL_TRADING_PLAYER_MANAGER_H_

#include <godot_cpp/classes/node.hpp>

#include "../core/utils.h"

namespace godot::CL {

class Player;

class PlayerManager : public Node {
    GDCLASS(PlayerManager, Node)

   private:
    bool debug_;
    Dictionary players_;

   protected:
    static void _bind_methods();

   public:
    PlayerManager();
    ~PlayerManager();

    DEBUG_METHODS()

    Player *get_player(const StringName n) const;
    Array get_players() const;

    void _enter_tree() override;
};
}  // namespace godot::CL

#endif  // CL_TRADING_PLAYER_MANAGER_H_
