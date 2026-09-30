@echo off
"C:\Cmake\bin\cmake.exe" -S "C:\ElecroBow" -B "C:\ElecroBow\build" -G "Visual Studio 17 2022" -A x64
echo CMAKE_EXIT_CODE=%ERRORLEVEL% > "C:\ElecroBow\_cmake_exit.txt"