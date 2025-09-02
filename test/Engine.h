// Engine.h
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
    enum class Mode { EDIT, PLAY };
    enum class ToolState { NONE, PLACING, REMOVING, CAMERA_SELECTION };

    Engine();
    ~Engine();

    void Init();
    void Run();
    void Shutdown();

private:
    Grid grid;
    std::unique_ptr<PlayerEntity> player;
    Mode currentMode = Mode::EDIT;
    ToolState currentTool = ToolState::NONE;

    // Camera selection variables
    Vector2 selectionStart = {0, 0};
    Vector2 selectionEnd = {0, 0};
    Rectangle selectedArea = {0, 0, 0, 0};
    Camera2D playerCamera = {0};
    bool showCameraConfirmation = false;

    void HandleEditModeInput();
    void HandlePlayerPlacement();
    void HandlePlayerRemoval();
    void HandlePlayerCameraSelection();
    void RenderModeControls();
    void RenderPlayerCameraSelectionUI();
    void ConfirmPlayerCameraSelection();
    void CancelPlayerCameraSelection();
    bool IsMouseOverUI() const;
};