#include "resource_manager.h"

#include "../core/utils.h"
#include "./tile_manager.h"

godot::CL::ResourceManager::ResourceManager()
    : TilePlaceable<ResourceTile>(PLACEABLE_RESOURCE) {}

godot::CL::ResourceManager::~ResourceManager() {}

//void godot::CL::ResourceManager::handle_entry_clicked_(StringName entry_name) {
//    emit_signal("resource_clicked", entry_name);
//}

void godot::CL::ResourceManager::_enter_tree() {
    Node *node{get_node_or_null("../TileManager")};
    setup_tile_manager_(node);
}

void godot::CL::ResourceManager::_exit_tree() {
    Utils::disconnect(static_cast<TileManager *>(tile_manager_), "ready",
                      tile_manager_ready_cb_);
    tile_manager_ = nullptr;
}

void godot::CL::ResourceManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("resource_notify_tile_manager"),
                         &ResourceManager::notify_tile_manager);

//    ClassDB::bind_method(D_METHOD("handle_resource_clicked_", "resource_name"),
//                         &ResourceManager::handle_entry_clicked_);

    ClassDB::bind_method(D_METHOD("lock_resource_buttons_except", "except"),
                         &ResourceManager::lock_buttons_except);
    ClassDB::bind_method(D_METHOD("unlock_resource_buttons_except", "except"),
                         &ResourceManager::unlock_buttons_except);

    ClassDB::bind_method(D_METHOD("lock_all_resource_buttons"),
                         &ResourceManager::lock_all_buttons);
    ClassDB::bind_method(D_METHOD("unlock_all_resource_buttons"),
                         &ResourceManager::unlock_all_buttons);

    ClassDB::add_signal("ResourceManager",
                        MethodInfo("resource_clicked",
                                   PropertyInfo(Variant::STRING_NAME, "name")));
}
