@echo off
if not exist build (
    mkdir build
)
gcc src/win_main.c -o build/browser.exe -lgdi32
