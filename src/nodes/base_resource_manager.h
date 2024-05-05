#ifndef CL_TRADING_BASE_RESOURCE_MANAGER_H_
#define CL_TRADING_BASE_RESOURCE_MANAGER_H_

#include <godot_cpp/classes/node.hpp>

#include "../core/utils.h"
#include "./base_resource.h"

namespace godot::CL {

class BaseResourceManager : public Node {
    GDCLASS(BaseResourceManager, Node)

   private:
    bool debug_;
    Dictionary resources_;

   protected:
    static void _bind_methods();

   public:
    BaseResourceManager();
    ~BaseResourceManager();

    DEBUG_METHODS()

    BaseResource *get_resource(const ResourceKind kind) const;
    Array get_resources() const;

    void _enter_tree() override;
};
}  // namespace godot::CL

#endif  // CL_TRADING_BASE_RESOURCE_MANAGER_H_
