#include "resource_manager.h"

#include "../core/utils.h"

// SIGNALS
const char *godot::CL::ResourceManager::SResourceClicked{"resource_clicked"};

godot::CL::ResourceManager::ResourceManager()
    : TilePlaceable(PLACEABLE_RESOURCE),
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
        if (!entries_.has(resource_name)) {
            entries_[resource_name] = root;
            Utils::connect(root, ResourceTile::SButtonClicked,
                           resource_clicked_cb_);
        }
    }
}

godot::CL::ResourceTile *godot::CL::ResourceManager::get_resource(
    StringName name) const {
    auto resource{cast_to<ResourceTile>(entries_[name])};
    if (!resource) {
        return nullptr;
    }
    return resource;
}

void godot::CL::ResourceManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_resource", "name"),
                         &ResourceManager::get_resource);
    ClassDB::bind_method(
        D_METHOD("get_resources_within_distance", "from", "distance"),
        &ResourceManager::get_resources_within_distance);
    ClassDB::bind_method(D_METHOD("get_resources"),
                         &ResourceManager::get_resources);
    ClassDB::bind_method(D_METHOD("handle_resource_clicked_", "resource_name"),
                         &ResourceManager::handle_resource_clicked_);

    ClassDB::add_signal(
        "ResourceManager",
        MethodInfo(SResourceClicked,
                   PropertyInfo(Variant::STRING_NAME, "resource_name")));
}
