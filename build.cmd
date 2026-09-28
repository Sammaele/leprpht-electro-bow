@echo off
setlocal

set "PROJECT_DIR=%~dp0"

if not exist "%PROJECT_DIR%build\CMakeCache.txt" (
    call "%PROJECT_DIR%configure.cmd" "%~1"
    if errorlevel 1 exit /b 1
)

cmake --build "%PROJECT_DIR%build" --config Release
