#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "Grid.h"
#include "Entities/PlayerEntity.h"

class Engine {
public:
    Engine();
    ~Engine();

    void Init();
    void Run();
    void Shutdown();

private:
    Grid grid;
    std::unique_ptr<PlayerEntity> player; // Using smart pointer for better memory management

    void HandlePlayerPlacement();
};