#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "../nodes/city.h"
#include "../nodes/city_manager.h"
#include "../nodes/route.h"
#include "../nodes/tile_manager.h"
#include "../nodes/trading_vehicle.h"

using namespace godot;

void initialize_cl_trading_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    ClassDB::register_class<CL::Utils>();
    ClassDB::register_class<CL::City>();
    ClassDB::register_class<CL::TradingVehicle>();
    ClassDB::register_class<CL::Route>();
    ClassDB::register_class<CL::TileManager>();
    ClassDB::register_class<CL::CityManager>();
}

void uninitialize_cl_trading_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT
cl_trading_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                        const GDExtensionClassLibraryPtr p_library,
                        GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address,
                                                   p_library, r_initialization);

    init_obj.register_initializer(initialize_cl_trading_module);
    init_obj.register_terminator(uninitialize_cl_trading_module);
    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
