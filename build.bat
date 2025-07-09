@echo off
@REM Build system for msys2 g++

@REM Uncomment this section to rebuild imgui and overlay base

@REM g++ -O3 -std=c++17 -c osimgui/imgui/*.cpp -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lgdi32 -lopengl32 -ld3d11 -ldxgi -ldwmapi -luser32 -static-libgcc -static-libstdc++ -D_WINDOWS
@REM g++ -O3 -std=c++17 -c osimgui/Minhook/*.c -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lgdi32 -lopengl32 -ld3d11 -ldxgi -ldwmapi -luser32 -static-libgcc -static-libstdc++ -D_WINDOWS
@REM g++ -O3 -std=c++17 -c osimgui/*.cpp -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lgdi32 -lopengl32 -ld3d11 -ldxgi -ldwmapi -luser32 -static-libgcc -static-libstdc++ -D_WINDOWS
@REM move *.o build

g++ -O3 -std=c++17 -D_WINDOWS -municode ^
    -Iimgui -Iimgui/backends ^
    src/main.cpp ^
    build/*.o ^
    -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lkernel32 -lole32 ^
    -static-libgcc -static-libstdc++ ^
    -o Overlay.exe