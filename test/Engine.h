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
#include "Entities/PlayerManager.h"
#include "Entities/EnemyEntity.h"
#include "Entities/EnemyManager.h"

class Engine {
public:
    enum class Mode { EDIT, PLAY };
    enum class ToolState { NONE, PLACING_PLAYER, PLACING_ENEMY,  REMOVING_PLAYER, REMOVING_ENEMY, CAMERA_SELECTION };

    Engine();
    ~Engine();

    void Init();
    void Run();
    void Shutdown();

private:
    Grid grid;
    PlayerManager playerManager;
    EnemyManager enemyManager;
    std::vector<std::unique_ptr<EnemyEntity>> enemies;
    std::unique_ptr<PlayerEntity> player;
    Mode currentMode = Mode::EDIT;
    ToolState currentTool = ToolState::NONE;

    RenderTexture2D playModeTexture;

    // Camera selection variables
    Vector2 selectionStart = {0, 0};
    Vector2 selectionEnd = {0, 0};
    Rectangle selectedArea = {0, 0, 0, 0};
    Camera2D playerCamera;
    bool showCameraConfirmation = false;
    bool playModeWindowOpen = false;

    // Player Input Handling
    void HandlePlayerPlacement();
    void HandlePlayerRemoval();
    // =======================================
    //Player Camera Selection Handling
    void HandlePlayerCameraSelection();
    void ConfirmPlayerCameraSelection();
    void CancelPlayerCameraSelection();
    // Enemy Input Handling
    void HandleEnemyPlacement();
    void HandleEnemyRemoval();
    // =======================================
    // UI Rendering
    void HandleEditModeInput();
    void RenderModeControls();
    void RenderPlayerCameraSelectionUI();
    // =======================================
    bool IsMouseOverUI() const;
    void ResetTool();
    void RenderPlayModeWindow();
};