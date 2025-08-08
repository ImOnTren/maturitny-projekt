#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "raymath.h"

class Grid {
public:
    Grid();
    ~Grid();

    void Update();
    void Draw();

    void RenderSizeAvailability();

    int GetTileSize() const {
        switch (selectedTileSizeIndex) {
            case 0: return 16;
            case 1: return 32;
            case 2: return 64;
            default: return 32;
        }
    }

private:
    int rows;
    int cols;

    Camera2D camera = { 0 };
    float zoom = 1.0f;

    std::vector<std::string> tileSize = { "16x16", "32x32", "64x64" };
    int selectedTileSizeIndex = 0; // 0 - 16x16, 1 - 32x32, 2 - 64x64

    RenderTexture2D gridTexture;
    bool needsRedraw = true;
    bool initialized = false;
};
