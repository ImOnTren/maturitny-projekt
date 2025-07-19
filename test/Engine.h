#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "Grid.h"

class Engine {
public:
    Engine();
    ~Engine();

    void Init();
    void Run();
    void Shutdown();

private:
    Grid grid;
};
