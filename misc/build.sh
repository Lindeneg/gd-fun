#!/usr/bin/env bash

flags=(
    compiledb=no
    city_debug=yes
    route_debug=yes
)

f=""
for t in ${flags[@]}; do
    f="${f} ${t}"
done

cmd="scons -j14 platform=windows arch=x86_64 target=template_debug debug_symbols=yes ${f}"

echo running $cmd

$cmd

