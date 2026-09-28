# ElectroBow - minimal VST3 plugin skeleton
cmake_minimum_required(VERSION 3.22)

project(ElectroBow VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Point this at a JUCE checkout when configuring, for example:
#   cmake -S . -B build -DJUCE_PATH=/path/to/JUCE
set(JUCE_PATH "" CACHE PATH "Path to the JUCE source tree")

if(NOT JUCE_PATH)
    message(FATAL_ERROR
        "ElectroBow: JUCE was not found. Set JUCE_PATH to the JUCE source tree.")
endif()

if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt")
    message(FATAL_ERROR
        "ElectroBow: JUCE CMakeLists.txt was not found at '${JUCE_PATH}'.")
endif()

add_subdirectory("${JUCE_PATH}" JUCE)

# ---------------------------------------------------------------------------
# aubio 0.4.9 - vendored in ThirdParty/aubio-src.
#
# Minimal static target for pitch detection using aubio's YIN algorithm.
# FFTW is not used. pitchyinfft.c is intentionally excluded.

set(AUBIO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/aubio-src"
    CACHE PATH "Path to the vendored aubio 0.4.9 source tree")

if(NOT EXISTS "${AUBIO_SRC_DIR}/src/aubio.h")
    message(FATAL_ERROR
        "ElectroBow: aubio sources not found at '${AUBIO_SRC_DIR}/src/aubio.h'. "
        "Expected layout: ThirdParty/aubio-src/src/aubio.h")
endif()

# Minimal replacement for aubio's waf-generated config.h.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/aubio_config/config.h" [=[
#ifndef AUBIO_CONFIG_H
#define AUBIO_CONFIG_H

#define _USE_MATH_DEFINES 1

#include <math.h>
#include <stdint.h>

#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MATH_H 1
#define HAVE_MATH_FUNC 1
#define HAVE_LOCALE_H 1
#define HAVE_STRINGS_H 1
#define HAVE_TIME_H 1
#define HAVE_STDARG_H 1
#define HAVE_C99_VARARGS_MACROS 1

#define HAVE_UNISTD_H 0
#define HAVE_GETOPT_H 0
#define HAVE_WINSOCK2_H 0

#define HAVE_SNDFILE_H 0
#define HAVE_SAMPLERATE_H 0

#define HAVE_AUBIO_DOUBLE 0

#define HAVE_INTERNAL_FFTW_H 0
#define HAVE_OOURA 0
#define HAVE_KISS_FFT 0

#define HAVE_GSL 0
#define HAVE_JACK 0
#define HAVE_PULSEAUDIO 0
#define HAVE_PORTAUDIO 0
#define HAVE_RTMIDI 0
#define HAVE_MIDI 0

#define HAVE_SOURCE_APPLE 0
#define HAVE_SOURCE_JACK 0
#define HAVE_SOURCE_PULSEAUDIO 0
#define HAVE_SOURCE_WASAPI 0
#define HAVE_SOURCE_COREAUDIO 0
#define HAVE_SOURCE_WINHS 0
#define HAVE_SOURCE_AUDIO_UNIT 0

#define HAVE_SINK_APPLE 0
#define HAVE_SINK_JACK 0
#define HAVE_SINK_PULSEAUDIO 0
#define HAVE_SINK_WINMM 0
#define HAVE_SINK_SOUNDIO 0

#endif
]=])

# ---------------------------------------------------------------------------
# Minimal aubio source set for YIN pitch detection.

set(AUBIO_SOURCES
    "${AUBIO_SRC_DIR}/src/fvec.c"
    "${AUBIO_SRC_DIR}/src/mathutils.c"
    "${AUBIO_SRC_DIR}/src/musicutils.c"
    "${AUBIO_SRC_DIR}/src/vecutils.c"
    "${AUBIO_SRC_DIR}/src/pitch/pitchyin.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/aubio_log.c"
)

add_library(aubio_static STATIC ${AUBIO_SOURCES})

# The static library is linked into the shared VST3 module on Unix platforms.
set_target_properties(aubio_static
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(aubio_static
    PUBLIC
        "${AUBIO_SRC_DIR}/src"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/aubio_config")

target_compile_definitions(aubio_static
    PRIVATE
        HAVE_CONFIG_H=1)

# ---------------------------------------------------------------------------
# STK 5.0.1 - minimal bowed-string model.
#
# Only the classes required by stk::Bowed are compiled.
#
# Expected layout:
# ThirdParty/stk/include/Bowed.h
# ThirdParty/stk/src/Bowed.cpp

set(STK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stk"
    CACHE PATH "Path to the vendored STK source tree")

if(NOT EXISTS "${STK_SRC_DIR}/include/Bowed.h")
    message(FATAL_ERROR
        "ElectroBow: STK Bowed.h not found at '${STK_SRC_DIR}/include/Bowed.h'. "
        "Expected layout: ThirdParty/stk/include/Bowed.h")
endif()

set(STK_SOURCES
    "${STK_SRC_DIR}/src/Stk.cpp"
    "${STK_SRC_DIR}/src/DelayL.cpp"
    "${STK_SRC_DIR}/src/OnePole.cpp"
    "${STK_SRC_DIR}/src/BiQuad.cpp"
    "${STK_SRC_DIR}/src/SineWave.cpp"
    "${STK_SRC_DIR}/src/ADSR.cpp"
    "${STK_SRC_DIR}/src/Bowed.cpp"
)

add_library(stk_bowed STATIC ${STK_SOURCES})

# The static library is linked into the shared VST3 module on Unix platforms.
set_target_properties(stk_bowed
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(stk_bowed
    PUBLIC
        "${STK_SRC_DIR}/include")

target_compile_features(stk_bowed
    PRIVATE
        cxx_std_17)
target_compile_definitions(stk_bowed
    PRIVATE
        _USE_MATH_DEFINES=1)
# ---------------------------------------------------------------------------
# Build only the VST3 format for this stage.

juce_add_plugin(ElectroBow
    COMPANY_NAME "ElectroBow"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Elcb
    PLUGIN_CODE Elbw
    FORMATS VST3
    PRODUCT_NAME "ElectroBow")

# ---------------------------------------------------------------------------
# Third-party include directories.

target_include_directories(ElectroBow
    PRIVATE
        "${AUBIO_SRC_DIR}/src"
        "${STK_SRC_DIR}/include")

# ---------------------------------------------------------------------------
# ElectroBow source files.

target_sources(ElectroBow
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
        PitchDetector.h)

target_compile_features(ElectroBow
    PRIVATE
        cxx_std_17)

# ---------------------------------------------------------------------------
# Keep the third-party dependency footprint at zero.

target_compile_definitions(ElectroBow
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# ---------------------------------------------------------------------------
# Link JUCE, aubio and STK bowed-string model.

target_link_libraries(ElectroBow
    PRIVATE
        juce::juce_audio_utils
        aubio_static
        stk_bowed
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
