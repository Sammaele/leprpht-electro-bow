# ElectroBow

ElectroBow is a JUCE-based VST3 audio plugin that detects the pitch of incoming audio and emits MIDI notes.

## Requirements

- CMake 3.22 or newer
- A JUCE source checkout (JUCE is not bundled with this repository)
- A C++17 compiler

On Windows, install Visual Studio with the **Desktop development with C++** workload. On macOS, install Xcode or the Xcode Command Line Tools. On Linux, install GCC or Clang and the usual build tools for your distribution.

## Build

The build scripts use the same workflow on all supported platforms. Pass the path to your JUCE checkout the first time:

macOS/Linux:

```sh
./build.sh /path/to/JUCE
```

Windows:

```bat
build.cmd C:\path\to\JUCE
```

The script configures the project automatically when needed and builds the `Release` configuration. Subsequent builds need no JUCE argument because the path is stored in the CMake build directory:

```sh
./build.sh       # macOS/Linux
build.cmd        # Windows
```

If a JUCE checkout is placed in a `JUCE/` folder beside the project, the scripts detect it automatically. You can also set `JUCE_PATH` once instead of passing it as an argument:

```sh
export JUCE_PATH=/path/to/JUCE       # macOS/Linux
set JUCE_PATH=C:\path\to\JUCE       # Windows Command Prompt
```

For direct CMake use, the equivalent commands are:

```sh
cmake -S . -B build -DJUCE_PATH=/path/to/JUCE
cmake --build build --config Release
```

The generated VST3 plugin is placed under `build/ElectroBow_artefacts/Release/VST3/` (the exact bundle/file layout varies slightly by platform).

Visual Studio is only used as the compiler/generator on Windows; no solution file is required or checked into the repository. CMake will select an appropriate native generator unless one is specified explicitly.

## Project layout

- `PluginProcessor.*` and `PluginEditor.*` contain the plugin implementation.
- `PitchDetector.h` contains the aubio-based pitch detector.
- `ThirdParty/` contains the vendored aubio and STK sources used by the plugin.
- `CMakeLists.txt` defines the platform-independent build.

The vendored dependencies retain their upstream licenses in `ThirdParty/aubio-src/COPYING` and `ThirdParty/stk/LICENSE`.
