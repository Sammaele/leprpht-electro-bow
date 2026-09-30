@echo off
setlocal

cd /d C:\ElecroBow

if not exist build (
    echo Configuring project...
    cmake -S . -B build -G "Visual Studio 17 2022" -A x64
    if errorlevel 1 (
        echo CMake configuration failed.
        pause
        exit /b 1
    )
)

echo Building ElectroBow...
cmake --build build --config Release

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESSFUL
echo.
pause
