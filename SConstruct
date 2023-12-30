#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/", "src/core/", "src/nodes/"])
sources = Glob("src/**/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "cl-trading/bin/cltrading.{}.{}.framework/cltrading.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "cl-trading/bin/cltrading{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
