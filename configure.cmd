@echo off
setlocal

set "PROJECT_DIR=%~dp0"
if not "%~1"=="" set "JUCE_PATH=%~1"

if "%JUCE_PATH%"=="" if exist "%PROJECT_DIR%JUCE\CMakeLists.txt" set "JUCE_PATH=%PROJECT_DIR%JUCE"

if "%JUCE_PATH%"=="" (
    echo Usage: configure.cmd C:\path\to\JUCE
    echo Or set JUCE_PATH to your JUCE source tree first.
    exit /b 1
)

cmake -S "%PROJECT_DIR%" -B "%PROJECT_DIR%build" -DJUCE_PATH="%JUCE_PATH%"
