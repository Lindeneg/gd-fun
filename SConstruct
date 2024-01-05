#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/core/", "src/nodes/", "src/register"])

if env["target"] == "template_debug" or env["target"] == "editor":
    env.Append(CPPDEFINES=["CL_TRADING_DEBUG=1"])

sources = Glob("src/**/*.cpp")
library = env.SharedLibrary(
        "cl-trading/bin/cltrading{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources)


Default(library)
