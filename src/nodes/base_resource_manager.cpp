#include "base_resource_manager.h"

#ifdef CL_TRADING_DEBUG
MAKE_M_LOG(BRESOURCEMLOG, BaseResourceManager)
#endif

godot::CL::BaseResourceManager::BaseResourceManager()
    : debug_(false), resources_(Dictionary()) {}

godot::CL::BaseResourceManager::~BaseResourceManager() {}

void godot::CL::BaseResourceManager::_enter_tree() {
    TypedArray<Node> children{get_children()};
    for (int i = 0; i < children.size(); i++) {
        BaseResource *resource{cast_to<BaseResource>(children[i])};
        ERR_FAIL_NULL(resource);
#ifdef CL_TRADING_DEBUG
        BRESOURCEMLOG(
            debug_, "found base-resource name=%s,weight=%d,value=%d\n",
            GDSTR(resource), resource->get_weight(), resource->get_value());
#endif
        resources_[resource->get_kind()] = resource;
    }
}

godot::CL::BaseResource *godot::CL::BaseResourceManager::get_resource(
    const ResourceKind kind) const {
    if (resources_.has(kind)) {
        return cast_to<BaseResource>(resources_[kind]);
    }
    return nullptr;
}

godot::Array godot::CL::BaseResourceManager::get_resources() const {
    Array keys{resources_.keys()};
    Array result{};
    for (int i = 0; i < keys.size(); i++) {
        BaseResource *resource{cast_to<BaseResource>(resources_[keys[i]])};
        ERR_FAIL_NULL_V(resource, result);
        result.append(resource);
    }
    return result;
}

void godot::CL::BaseResourceManager::_bind_methods() {
    DEBUG_BIND(BaseResourceManager)

    ClassDB::bind_method(D_METHOD("get_resource", "k"),
                         &BaseResourceManager::get_resource);
    ClassDB::bind_method(D_METHOD("get_resources"),
                         &BaseResourceManager::get_resources);
}
