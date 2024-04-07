#ifndef CL_TRADING_BASE_RESOURCE_H_
#define CL_TRADING_BASE_RESOURCE_H_

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot::CL {
enum ResourceKind {
    RESOURCE_PASSENGER,  // STANDARD RESOURCE
    RESOURCE_MAIL,       // STANDARD RESOURCE
    RESOURCE_GRAIN,      //   TO -> FOOD, BEER
    RESOURCE_FISH,       //   TO -> FOOD
    RESOURCE_ROCK,       //   TO -> COBBLE, TOOLS
    RESOURCE_IRON,       //   TO -> STEEL, TOOLS
    RESOURCE_WOOD,       //   TO -> PAPER, TOOLS
    RESOURCE_BOAR,       //   TO -> FOOD
    RESOURCE_PIG,        //   TO -> FOOD
    RESOURCE_CHICKEN,    //   TO -> FOOD
    RESOURCE_SHEEP,      //   TO -> FOOD, TEXTILES
    RESOURCE_POPPY,      //   TO -> MEDICINE
    RESOURCE_FOOD,       // FROM -> GRAIN, FISH, BOAR, PIG, CHICKEN, SHEEP
    RESOURCE_BEER,       // FROM -> GRAIN
    RESOURCE_COBBLE,     // FROM -> ROCK
    RESOURCE_TOOLS,      // FROM -> ROCK, IRON, WOOD
    RESOURCE_TEXTILES,   // FROM -> SHEEP
    RESOURCE_MEDICINE    // FROM -> POPPY
};

class BaseResource : public Node2D {
    GDCLASS(BaseResource, Node2D)

   private:
    ResourceKind kind_;
    int value_;
    int weight_;
    Array value_history_;

   protected:
    static void _bind_methods();

   public:
    BaseResource();
    ~BaseResource();

    inline ResourceKind get_kind() const { return kind_; }
    inline int get_value() const { return value_; }
    inline int get_weight() const { return weight_; }
    inline Array get_value_history() const { return value_history_; }

    inline void set_kind(const ResourceKind r) { kind_ = r; }
    inline void set_value(const int v) { value_ = v; }
    inline void set_weight(const int w) { weight_ = w; }

    void _ready() override;
};
}  // namespace godot::CL
   //
VARIANT_ENUM_CAST(godot::CL::ResourceKind);

#endif  // CL_TRADING_BASE_RESOURCE_H_
