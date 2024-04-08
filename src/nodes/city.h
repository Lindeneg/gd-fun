#ifndef CL_TRADING_CITY_H_
#define CL_TRADING_CITY_H_

#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "../core/entryable.h"

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
class City : public Entryable {
    GDCLASS(City, Entryable)

   private:
    CitySize size_;

   protected:
    static void _bind_methods();

   public:
    City();
    ~City();

    void _ready() override;

    inline CitySize get_size() const { return size_; }
    inline void set_size(const CitySize s) { size_ = s; }
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::CitySize);

#endif  // CL_TRADING_CITY_H_
