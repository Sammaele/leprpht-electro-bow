#!/usr/bin/env sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
build_dir=${BUILD_DIR:-"$project_dir/build"}
build_config=${BUILD_CONFIG:-Release}

if [ ! -f "$build_dir/CMakeCache.txt" ]; then
    "$project_dir/configure.sh" "${1:-${JUCE_PATH:-}}"
fi

cmake --build "$build_dir" --config "$build_config"
