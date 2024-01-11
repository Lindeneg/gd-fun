#ifndef CL_TRADING_CITY_H_
#define CL_TRADING_CITY_H_

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot {
class CollisionShape2D;
}  // namespace godot

namespace godot::CL {

enum CitySize {
    CITY_SIZE_VILLAGE,
    CITY_SIZE_TOWN,
    CITY_SIZE_URBAN,
    CITY_SIZE_REGIO,
    CITY_SIZE_METRO
};

enum CityEntryType { CITY_ENTRY_ONSHORE = 1, CITY_ENTRY_OFFSHORE = 2 };

/* City has supply, demand and a size.
 * City also only place where
 * Industry can be located.
 *
 * City size grows over time allowing
 * for more supply, demand and Industry. */
class City : public Area2D {
    GDCLASS(City, Area2D)

   private:
    CollisionShape2D* col_shape_;
    // TODO (2) use typed array
    // resource supply
    Array supply_;
    // resource demand
    Array demand_;
    // industries in city
    Array industries_;
    // onshore entry points for trading vehicles
    Array onshore_entries_;
    // offshore entry points for trading vehicles
    Array offshore_entries_;
    // city size
    CitySize size_;
    // maximum capacity i.e
    // how many concurrent routes
    int max_route_capacity_;
    // current route size
    int current_route_size_;

    void r_assign_required_components_();
    void e_assign_required_components_();

   protected:
    static void _bind_methods();

   public:
    City();
    ~City();

    void _ready() override;

    // TODO (2)
    // void add_supply();
    // void remove_supply();
    // void add_demand();
    // void remove_demand();
    // void add_industry();
    // void remove_industry();
    inline void set_size(const CitySize s) { size_ = s; }

    void add_city_entry_point(const Vector2i coords, const CityEntryType type);

    inline Array get_onshore_entries() const { return onshore_entries_; }
    inline Array get_offshore_entries() const { return offshore_entries_; }
    inline Array get_supply() const { return supply_; }
    inline Array get_demand() const { return demand_; }
    inline Array get_industries() const { return industries_; }
    inline CitySize get_size() const { return size_; }
    inline int get_route_capacity() const { return max_route_capacity_; }
    inline int get_current_route_size() const { return current_route_size_; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::CitySize);
VARIANT_ENUM_CAST(godot::CL::CityEntryType);

#endif  // CL_TRADING_CITY_H_
