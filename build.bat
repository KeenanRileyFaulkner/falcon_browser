@echo off
mkdir build
g++ src/win_main.cpp -o build/browser.exe -lgdi32
