#include "player.h"

#include <godot_cpp/classes/global_constants.hpp>

#ifdef CL_TRADING_DEBUG
MAKE_LOG(PLAYERLOG, Player)
#endif

godot::CL::Player::Player()
    : debug_(false),
      connections_(Array()),
      level_(PLAYER_LEVEL_NOVICE),
      finance_(nullptr),
      is_active_(true) {}

godot::CL::Player::~Player() {
    Utils::queue_delete(finance_);
    finance_ = nullptr;
}

void godot::CL::Player::r_assign_required_components_() {
    ERR_FAIL_COND_MSG(
        finance_ != nullptr,
        vformat("component PlayerFinance already assigned for player %s",
                get_name()));
    Node *node{find_child("*Finance*")};
    ERR_FAIL_COND_MSG(
        node == nullptr,
        vformat("component PlayerFinance not found for player %s", get_name()));
    finance_ = static_cast<PlayerFinance *>(node);
    finance_->initialize_finance(get_name());
}

void godot::CL::Player::_ready() {
    if (!Utils::is_in_editor()) {
        r_assign_required_components_();
    }
}

bool godot::CL::Player::has_connection(const StringName n) {
    if (n == start_city_) {
        return true;
    }
    for (int i = 0; i < connections_.size(); i++) {
        Dictionary connection{connections_[i]};
        if (static_cast<StringName>(connection["name"]) == n) {
            return static_cast<int>(connection["count"]) > 0;
        }
    }
    return false;
}

void godot::CL::Player::add_connection(const StringName n) {
    for (int i = 0; i < connections_.size(); i++) {
        Dictionary connection{connections_[i]};
        if (static_cast<StringName>(connection["name"]) == n) {
            int new_amount{static_cast<int>(connection["count"]) + 1};
            connection["count"] = new_amount;
#ifdef CL_TRADING_DEBUG
            PLAYERLOG(this, "existing connection added %s, total %d\n",
                      GDSTR(n), new_amount);
#endif
            return;
        }
    }
    Dictionary connection{};
    connection["name"] = n;
    connection["count"] = 1;
    connections_.append(connection);
#ifdef CL_TRADING_DEBUG
    PLAYERLOG(this, "new connection added %s\n", GDSTR(n));
#endif
    emit_signal("connection_added", n);
}
void godot::CL::Player::remove_connection(const StringName n) {
    for (int i = 0; i < connections_.size(); i++) {
        Dictionary connection{connections_[i]};
        if (static_cast<StringName>(connection["name"]) == n) {
            int new_amount{
                Math::max(0, static_cast<int>(connection["count"]) - 1)};
            connection["count"] = new_amount;
            if (new_amount == 0) {
#ifdef CL_TRADING_DEBUG
                PLAYERLOG(this, "connection removed %s\n", GDSTR(n));
#endif
                emit_signal("connection_removed", n);
            } else {
#ifdef CL_TRADING_DEBUG
                PLAYERLOG(this, "existing connection removed %s, total %d\n",
                          GDSTR(n), new_amount);
#endif
            }
            return;
        }
    }
}

void godot::CL::Player::_bind_methods() {
    DEBUG_BIND(Player)

    ClassDB::bind_method(D_METHOD("get_finance"), &Player::get_finance);

    ClassDB::bind_method(D_METHOD("get_connections"), &Player::get_connections);
    ClassDB::bind_method(D_METHOD("has_connection", "n"),
                         &Player::has_connection);
    ClassDB::bind_method(D_METHOD("add_connection", "n"),
                         &Player::add_connection);
    ClassDB::bind_method(D_METHOD("remove_connection", "n"),
                         &Player::remove_connection);
    ClassDB::bind_method(D_METHOD("set_connections", "c"),
                         &Player::set_connections);
    ClassDB::bind_method(D_METHOD("get_start_city"), &Player::get_start_city);
    ClassDB::bind_method(D_METHOD("set_start_city", "c"),
                         &Player::set_start_city);
    ClassDB::bind_method(D_METHOD("get_player_level"),
                         &Player::get_player_level);
    ClassDB::bind_method(D_METHOD("set_player_level", "l"),
                         &Player::set_player_level);
    ClassDB::bind_method(D_METHOD("get_range"), &Player::get_range);
    ClassDB::bind_method(D_METHOD("get_is_active"), &Player::get_is_active);
    ClassDB::bind_method(D_METHOD("set_is_active", "a"),
                         &Player::set_is_active);

    ClassDB::add_property("Player",
                          PropertyInfo(Variant::STRING_NAME, "start_city"),
                          "set_start_city", "get_start_city");

    ClassDB::add_property(
        "Player",
        PropertyInfo(Variant::INT, "level", PROPERTY_HINT_ENUM,
                     "Novice,Intermediate,Advanced,Master"),
        "set_player_level", "get_player_level");

    BIND_ENUM_CONSTANT(PLAYER_LEVEL_NOVICE);
    BIND_ENUM_CONSTANT(PLAYER_LEVEL_INTERMEDIATE);
    BIND_ENUM_CONSTANT(PLAYER_LEVEL_ADVANCED);
    BIND_ENUM_CONSTANT(PLAYER_LEVEL_MASTER);

    ClassDB::add_signal("Player",
                        MethodInfo("connection_added",
                                   PropertyInfo(Variant::STRING_NAME, "name")));
    ClassDB::add_signal("Player",
                        MethodInfo("connection_removed",
                                   PropertyInfo(Variant::STRING_NAME, "name")));
}
