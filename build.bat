@echo off
echo ============================================
echo   Shadow Engine 2D - Build Script
echo ============================================

set GCC=C:\msys64\ucrt64\bin\g++.exe
set CFLAGS=-std=c++17 -Wall -O2 -Iinclude -IC:/msys64/ucrt64/include/SDL2 -IC:/msys64/ucrt64/include
set LDFLAGS=-LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

echo [1/2] Compiling...
%GCC% %CFLAGS% ^
    src/main.cpp ^
    src/Game.cpp ^
    src/Timer.cpp ^
    src/TextureManager.cpp ^
    src/InputManager.cpp ^
    src/Animation.cpp ^
    src/Camera.cpp ^
    src/GameObject.cpp ^
    src/Player.cpp ^
    src/TileMap.cpp ^
    src/Item.cpp ^
    src/Inventory.cpp ^
    src/TimeSystem.cpp ^
    src/Enemy.cpp ^
    -o game2d.exe ^
    %LDFLAGS%

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo [2/2] Build successful!
echo.
echo Run: game2d.exe
pause
