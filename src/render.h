#pragma once

#include "inc.h"

bool menuOpen = true;

#include <chrono>
#include <iostream>
#include <functional>

void DrawCallBack() {
    ImGui::Begin("Test");
    ImGui::End();
}