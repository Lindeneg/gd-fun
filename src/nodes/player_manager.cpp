#include "player_manager.h"

#include "./player.h"

#ifdef CL_TRADING_DEBUG
MAKE_M_LOG(PLAYERMLOG, PlayerManager)
#endif

godot::CL::PlayerManager::PlayerManager()
    : debug_(false), players_(Dictionary()) {}

godot::CL::PlayerManager::~PlayerManager() {}

void godot::CL::PlayerManager::_enter_tree() {
    TypedArray<Node> children{get_children()};
    for (int i = 0; i < children.size(); i++) {
        Player *player{cast_to<Player>(children[i])};
        ERR_FAIL_NULL(player);
#ifdef CL_TRADING_DEBUG
        PLAYERMLOG(debug_, "found player %s\n", GDSTR(player));
#endif
        players_[player->get_name()] = player;
    }
}

godot::CL::Player *godot::CL::PlayerManager::get_player(
    const StringName n) const {
    if (players_.has(n)) {
        return cast_to<Player>(players_[n]);
    }
    return nullptr;
}

godot::Array godot::CL::PlayerManager::get_players() const {
    Array keys{players_.keys()};
    Array result{};
    for (int i = 0; i < keys.size(); i++) {
        Player *player{cast_to<Player>(players_[keys[i]])};
        ERR_FAIL_NULL_V(player, Array());
        result.append(player);
    }
    return result;
}

void godot::CL::PlayerManager::_bind_methods() {
    DEBUG_BIND(PlayerManager)

    ClassDB::bind_method(D_METHOD("get_player", "n"),
                         &PlayerManager::get_player);
    ClassDB::bind_method(D_METHOD("get_players"), &PlayerManager::get_players);
}

