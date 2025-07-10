@echo off

echo Rebuilding Dependencies...
<nul set /p=Building ImGui         [---------] 0/3
g++ -O3 -std=c++17 -c osimgui/imgui/*.cpp -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lgdi32 -lopengl32 -ld3d11 -ldxgi -ldwmapi -luser32 -static-libgcc -static-libstdc++ -D_WINDOWS -Wpragma-once-outside-header
powershell -command "[console]::SetCursorPosition(0,[console]::CursorTop)"
<nul set /p=Building Minhook       [###------] 1/3
g++ -O3 -std=c++17 -c osimgui/Minhook/*.c -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lgdi32 -lopengl32 -ld3d11 -ldxgi -ldwmapi -luser32 -static-libgcc -static-libstdc++ -D_WINDOWS -Wpragma-once-outside-header
powershell -command "[console]::SetCursorPosition(0,[console]::CursorTop)"
<nul set /p=Building Os-ImGui      [######---] 2/3
g++ -O3 -std=c++17 -c osimgui/*.cpp -lgdi32 -lopengl32 -ld3d11 -ldxgi -ld3dcompiler -ldwmapi -luser32 -lgdi32 -lopengl32 -ld3d11 -ldxgi -ldwmapi -luser32 -static-libgcc -static-libstdc++ -D_WINDOWS -Wpragma-once-outside-header
powershell -command "[console]::SetCursorPosition(0,[console]::CursorTop)"
echo Rebuilt Dependencies   [######---] 3/3 [OK]
move *.o build