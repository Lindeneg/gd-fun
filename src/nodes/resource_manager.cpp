#include "resource_manager.h"

#include "../core/utils.h"

// SIGNALS
const char *godot::CL::ResourceManager::SResourceClicked{"resource_clicked"};

godot::CL::ResourceManager::ResourceManager()
    : TilePlaceable(PLACEABLE_RESOURCE),
      resources_({}),
      resource_clicked_cb_(Callable(this, "handle_resource_clicked_")) {}

godot::CL::ResourceManager::~ResourceManager() {}

void godot::CL::ResourceManager::handle_resource_clicked_(
    StringName resource_name) {
    emit_signal(SResourceClicked, resource_name);
}

void godot::CL::ResourceManager::iterate_children_(
    TypedArray<Entryable> nodes) {
    for (auto i = 0; i < nodes.size(); i++) {
        auto root{cast_to<ResourceTile>(nodes[i])};
        handle_entryable_node_(root, root, nullptr);
        auto resource_name = root->get_name();
        if (resources_.find(resource_name) == resources_.end()) {
            resources_[resource_name] = root;
            Utils::connect(root, ResourceTile::SButtonClicked,
                           resource_clicked_cb_);
        }
    }
}

void godot::CL::ResourceManager::lock_all_buttons() {
    for (auto resource_el : resources_) {
        resource_el.second->set_button_enabled(false);
    }
}

void godot::CL::ResourceManager::unlock_all_buttons() {
    for (auto resource_el : resources_) {
        resource_el.second->set_button_enabled(true);
    }
}

void godot::CL::ResourceManager::lock_buttons_except(
    TypedArray<StringName> except) {
    for (auto resource_el : resources_) {
        resource_el.second->set_button_enabled(except.has(resource_el.first));
    }
}

void godot::CL::ResourceManager::unlock_buttons_except(
    TypedArray<StringName> except) {
    for (auto resource_el : resources_) {
        resource_el.second->set_button_enabled(!except.has(resource_el.first));
    }
}

godot::CL::ResourceTile *godot::CL::ResourceManager::get_resource(
    StringName name) const {
    auto resource = resources_.find(name);
    if (resource == resources_.end()) {
        return nullptr;
    }
    return resource->second;
}

godot::Array godot::CL::ResourceManager::get_resources_within_distance(
    Entryable *from, int distance) {
    return get_within_distance_(resources_, from, distance);
}

void godot::CL::ResourceManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_resource", "name"),
                         &ResourceManager::get_resource);
    ClassDB::bind_method(
        D_METHOD("get_resources_within_distance", "from", "distance"),
        &ResourceManager::get_resources_within_distance);
    ClassDB::bind_method(D_METHOD("handle_resource_clicked_", "resource_name"),
                         &ResourceManager::handle_resource_clicked_);

    ClassDB::bind_method(D_METHOD("lock_buttons_except", "except"),
                         &ResourceManager::lock_buttons_except);
    ClassDB::bind_method(D_METHOD("unlock_buttons_except", "except"),
                         &ResourceManager::unlock_buttons_except);

    ClassDB::bind_method(D_METHOD("lock_all_buttons"),
                         &ResourceManager::lock_all_buttons);
    ClassDB::bind_method(D_METHOD("unlock_all_buttons"),
                         &ResourceManager::unlock_all_buttons);

    ClassDB::add_signal(
        "ResourceManager",
        MethodInfo(SResourceClicked,
                   PropertyInfo(Variant::STRING_NAME, "resource_name")));
}
