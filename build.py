#!/usr/bin/env python3
"""Configure and build ElectroBow on any platform with Python and CMake."""

from pathlib import Path
import argparse
import os
import subprocess


PROJECT_DIR = Path(__file__).resolve().parent


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("juce_path", nargs="?", help="Path to the JUCE source tree")
    parser.add_argument("--build-dir", default="build", help="CMake build directory")
    parser.add_argument("--config", default="Release", help="Build configuration")
    args = parser.parse_args()

    build_dir = Path(args.build_dir)
    if not build_dir.is_absolute():
        build_dir = PROJECT_DIR / build_dir

    juce_path = args.juce_path or os.environ.get("JUCE_PATH")
    local_juce = PROJECT_DIR / "JUCE"
    if not juce_path and (local_juce / "CMakeLists.txt").is_file():
        juce_path = str(local_juce)

    if not juce_path:
        parser.error("provide JUCE_PATH or pass the JUCE path as the first argument")

    cache_file = build_dir / "CMakeCache.txt"
    if not cache_file.is_file():
        subprocess.run(
            [
                "cmake",
                "-S",
                str(PROJECT_DIR),
                "-B",
                str(build_dir),
                f"-DJUCE_PATH={juce_path}",
            ],
            check=True,
        )

    subprocess.run(
        ["cmake", "--build", str(build_dir), "--config", args.config],
        check=True,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
