#include "player.h"

godot::CL::Player::Player()
    : connections_(Array()),
      level_(PLAYER_LEVEL_NOVICE),
      gold_(100),
      range_(100),
      is_active_(true) {}

godot::CL::Player::~Player() {}

void godot::CL::Player::_ready() { connections_.push_back(start_city_); }

void godot::CL::Player::_bind_methods() {
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
    ClassDB::bind_method(D_METHOD("get_gold"), &Player::get_gold);
    ClassDB::bind_method(D_METHOD("set_gold", "g"), &Player::set_gold);
    ClassDB::bind_method(D_METHOD("get_range"), &Player::get_range);
    ClassDB::bind_method(D_METHOD("set_range", "r"), &Player::set_range);
    ClassDB::bind_method(D_METHOD("get_is_active"), &Player::get_is_active);
    ClassDB::bind_method(D_METHOD("set_is_active", "a"),
                         &Player::set_is_active);

    ClassDB::add_property("Player",
                          PropertyInfo(Variant::STRING_NAME, "start_city"),
                          "set_start_city", "get_start_city");

    ClassDB::add_property(
        "Player",
        PropertyInfo(Variant::INT, "gold", PROPERTY_HINT_RANGE, "0,1000"),
        "set_gold", "get_gold");

    ClassDB::add_property(
        "Player",
        PropertyInfo(Variant::INT, "range", PROPERTY_HINT_RANGE, "0,1000"),
        "set_range", "get_range");

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
