@echo off
@REM Build system for msys2 g++

set appname=Overlay.exe

if exist %appname% (
    del %appname%
    echo Deleted outdated binary!
)

@REM Uncomment this section to rebuild imgui and overlay base
@REM call rebuild_deps.bat

echo Building Program...

g++ -O3 -std=c++17 -D_WINDOWS -municode ^
    -Iimgui -Iimgui/backends ^
    src/main.cpp ^
    build/*.o ^
    -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lkernel32 -lole32 ^
    -static-libgcc -static-libstdc++ -Wpragma-once-outside-header ^
    -o %appname%

if exist %appname% (
    echo Build is finished! [OK]
)