# 0ms-Imgui

Based on [my fork of osimgui](https://github.com/wwidlishy/OS-ImGui-VsyncControll).  
[Original osimgui](https://github.com/TKazer/OS-ImGui)

This is a exterbal overlay over a program made for windows and dx11 is required  
Build script is for msys g++ but the code should work under visual studio  

Due to how the "transparency" works the overlay is unable to be captured by OBS or simmilar.  
If you want it to be capturable you have to modify the `DesktopCaptureAndDrawToImGui` function in [Os-ImGui_External.cpp](https://github.com/wwidlishy/0ms-Imgui/blob/main/osimgui/OS-ImGui_External.cpp)
