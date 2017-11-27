#!/bin/bash
set -e
set -x

if [[ -e build ]] ; then
    rm -rf build
fi

jobCount=-j$(getconf _NPROCESSORS_ONLN)

meson build --buildtype debugoptimized
ninja -C build $jobCount

# Normal
meson test -C build

# Valgrind
valgrindArgs="valgrind --error-exitcode=1"
meson test -C build --print-errorlogs --logbase=memcheck --wrap="$valgrindArgs --tool=memcheck --leak-check=full --show-reachable=no"
meson test -C build --print-errorlogs --logbase=helgrind --wrap="$valgrindArgs --tool=helgrind"
meson test -C build --print-errorlogs --logbase=drd --wrap="$valgrindArgs --tool=drd"
