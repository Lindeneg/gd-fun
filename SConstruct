#!/usr/bin/env python
import os
import sys

def debug_flag(flag):
    if ARGUMENTS.get(flag, "no") == "yes":
        pound_define = f"CL_TRADING_{flag.upper()}=1"
        print("Defining " + pound_define);
        env.Append(CPPDEFINES=[pound_define])

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/core/", "src/nodes/", "src/register"])

if env["target"] == "template_debug" or env["target"] == "editor":
    env.Append(CPPDEFINES=["CL_TRADING_DEBUG=1"])
    debug_flag("city_debug")
    debug_flag("route_debug")

sources = Glob("src/**/*.cpp")
library = env.SharedLibrary(
        "cl-trading/bin/cltrading{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources)


Default(library)
