#ifndef CL_TRADING_CITY_H_
#define CL_TRADING_CITY_H_

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot::CL {

enum CitySize {
    CITY_SIZE_VILLAGE,
    CITY_SIZE_TOWN,
    CITY_SIZE_URBAN,
    CITY_SIZE_REGIO,
    CITY_SIZE_METRO
};

/* City has supply, demand and a size.
 * City also only place where
 * Industry can be located.
 *
 * City size grows over time allowing
 * for more supply, demand and Industry. */
class City : public Node2D {
    GDCLASS(City, Node2D)

    // TODO create CityManager
   private:
    // TODO use typed array
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

   protected:
    static void _bind_methods();

   public:
    City();
    ~City();

    void _ready() override;

    // TODO
    // void add_supply();
    // void remove_supply();
    // void add_demand();
    // void remove_demand();
    // void add_industry();
    // void remove_industry();
    inline void set_size(const CitySize s) { size_ = s; }

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

#endif  // CL_TRADING_CITY_H_
