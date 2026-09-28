#!/usr/bin/env sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
juce_path=${1:-${JUCE_PATH:-}}
build_dir=${BUILD_DIR:-"$project_dir/build"}

if [ -z "$juce_path" ] && [ -f "$project_dir/JUCE/CMakeLists.txt" ]; then
    juce_path="$project_dir/JUCE"
fi

if [ -z "$juce_path" ]; then
    echo "Usage: $0 /path/to/JUCE" >&2
    echo "Or set JUCE_PATH to your JUCE source tree." >&2
    exit 1
fi

cmake -S "$project_dir" -B "$build_dir" -DJUCE_PATH="$juce_path"
