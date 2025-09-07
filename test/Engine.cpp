#include "Engine.h"

using namespace std;

Engine::Engine() : playerManager(grid), enemyManager(grid) { // Remove camera parameters
    playerCamera.offset = {0, 0};
    playerCamera.target = {0, 0};
    playerCamera.rotation = 0.0f;
    playerCamera.zoom = 1.0f;
}

Engine::~Engine() { }

void Engine::Init() {
    InitWindow(800, 400, "Game Engine");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    rlImGuiSetup(true);
}

bool Engine::IsMouseOverUI() const {
    // Check if mouse is over any ImGui window
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void Engine::HandleEditModeInput() {
    if (currentMode != Mode::EDIT || IsMouseOverUI()) return;

    switch (currentTool) {
        case ToolState::PLACING_PLAYER:
            HandlePlayerPlacement();
            break;
        case ToolState::REMOVING_PLAYER:
            HandlePlayerRemoval();
            break;
        case ToolState::PLACING_ENEMY:
            HandleEnemyPlacement();
            break;
        case ToolState::REMOVING_ENEMY:
            HandleEnemyRemoval();
            break;
        case ToolState::CAMERA_SELECTION:
            HandlePlayerCameraSelection();
            break;
        case ToolState::NONE:
            // Allow grid movement only when no tool is active
            grid.Update();
            break;
    }
}
// =======================================
// =      Camera Selection Logic        =
// =======================================
void Engine::HandlePlayerCameraSelection() {
    if (IsMouseOverUI()) return;

    Vector2 mouseScreen = GetMousePosition();
    Camera2D gridCamera = grid.GetCamera();
    Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, gridCamera);

    int tileSize = grid.GetTileSize();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Start selection
        selectionStart = {
            static_cast<float>(static_cast<int>(mouseWorld.x / tileSize) * tileSize),
            static_cast<float>(static_cast<int>(mouseWorld.y / tileSize) * tileSize)
        };
        selectionEnd = selectionStart;
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        // Update selection while dragging
        selectionEnd = {
            static_cast<float>(static_cast<int>(mouseWorld.x / tileSize) * tileSize),
            static_cast<float>(static_cast<int>(mouseWorld.y / tileSize) * tileSize)
        };

        // Calculate selected area
        selectedArea = {
            min(selectionStart.x, selectionEnd.x),
            min(selectionStart.y, selectionEnd.y),
            abs(selectionEnd.x - selectionStart.x) + tileSize,
            abs(selectionEnd.y - selectionStart.y) + tileSize
        };
    }
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        // Finished selecting, show confirmation
        showCameraConfirmation = true;
        currentTool = ToolState::NONE; // Return to no tool after selection
    }
}

void Engine::ConfirmPlayerCameraSelection() {
    // Set up the player camera to focus on the selected area
    playerCamera.target = {
        selectedArea.x + selectedArea.width / 2,
        selectedArea.y + selectedArea.height / 2
    };

    // Set a reasonable zoom level based on the selected area
    float zoomX = (static_cast<float>(GetScreenWidth()) * 0.75f) / selectedArea.width;
    float zoomY = (static_cast<float>(GetScreenHeight())) / selectedArea.height;
    playerCamera.zoom = fminf(zoomX, zoomY);

    // Set the offset to the center of the screen
    playerCamera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    TraceLog(LOG_INFO, "Player camera set to area: (%.0f, %.0f, %.0f, %.0f)",
             selectedArea.x, selectedArea.y, selectedArea.width, selectedArea.height);
}

void Engine::CancelPlayerCameraSelection() {
    // Reset selection to x = 0 y = 0 width = 0 height = 0
    selectionStart = {0, 0};
    selectionEnd = {0, 0};
    selectedArea = {0, 0, 0, 0};
}
// =======================================
// =            UI Rendering             =
// =======================================
void Engine::RenderPlayerCameraSelectionUI() {
    if (showCameraConfirmation) {
        ImGui::OpenPopup("Confirm Camera Selection");
        showCameraConfirmation = false;
    }

    if (ImGui::BeginPopupModal("Confirm Camera Selection", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Set player camera to selected area?");
        ImGui::Text("Size: %.0fx%.0f", selectedArea.width, selectedArea.height);

        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            ConfirmPlayerCameraSelection();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            CancelPlayerCameraSelection();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Engine::ResetTool() {
    currentTool = ToolState::NONE;
}

void Engine::RenderModeControls() {
    ImGui::Separator();
    ImGui::Text("Engine Mode:");

    if (ImGui::RadioButton("Edit Mode", currentMode == Mode::EDIT)) {
        currentMode = Mode::EDIT;
        ResetTool(); // Reset tool when switching modes
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Play Mode", currentMode == Mode::PLAY)) {
        currentMode = Mode::PLAY;
        ResetTool(); // Reset tool when switching modes
    }

    if (currentMode == Mode::EDIT) {
        ImGui::Separator();
        ImGui::Text("Edit Tools:");

        // Player placement/removal
        bool playerPlacingActive = (currentTool == ToolState::PLACING_PLAYER);
        if (ImGui::RadioButton("Place Player", playerPlacingActive)) {
            currentTool = playerPlacingActive ? ToolState::NONE : ToolState::PLACING_PLAYER;
        }
        ImGui::SameLine();
        bool playerRemovingActive = (currentTool == ToolState::REMOVING_PLAYER);
        if (ImGui::RadioButton("Remove Player", playerRemovingActive)) {
            currentTool = playerRemovingActive ? ToolState::NONE : ToolState::REMOVING_PLAYER;
        }
        // =======================================
        // Enemy placement/removal
        bool enemyPlacingActive = (currentTool == ToolState::PLACING_ENEMY);
        if (ImGui::RadioButton("Place Enemy", enemyPlacingActive)) {
            currentTool = enemyPlacingActive ? ToolState::NONE : ToolState::PLACING_ENEMY;
        }
        ImGui::SameLine();
        bool enemyRemovingActive = (currentTool == ToolState::REMOVING_ENEMY);
        if (ImGui::RadioButton("Remove Enemy", enemyRemovingActive)) {
            currentTool = enemyRemovingActive ? ToolState::NONE : ToolState::REMOVING_ENEMY;
        }
        // =======================================
        // Camera selection
        bool cameraSelectionActive = (currentTool == ToolState::CAMERA_SELECTION);
        if (ImGui::Button("Set Player Camera Area", ImVec2(180, 0))) {
            currentTool = cameraSelectionActive ? ToolState::NONE : ToolState::CAMERA_SELECTION;
            if (currentTool == ToolState::CAMERA_SELECTION) {
                ImGui::SetTooltip("Click and drag on grid to select camera area");
            }
        }
        if (cameraSelectionActive) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), " (Active)");
        }
        // =======================================
        // Show current tool status
        ImGui::Text("Current Tool: ");
        ImGui::SameLine();
        switch (currentTool) {
            case ToolState::NONE: ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "None"); break;
            case ToolState::PLACING_PLAYER: ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Placing Player"); break;
            case ToolState::REMOVING_PLAYER: ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Removing Player"); break;
            case ToolState::PLACING_ENEMY: ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Placing Enemy"); break;
            case ToolState::REMOVING_ENEMY: ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Removing Enemy"); break;
            case ToolState::CAMERA_SELECTION: ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "Selecting Camera Area"); break;
        }
    }
}
// =======================================
// =          Player Functions           =
// =======================================
void Engine::HandlePlayerPlacement() {
    if (IsMouseOverUI()) return;
    Vector2 mouseScreen = GetMousePosition();
    if (playerManager.TryPlacePlayer(mouseScreen, grid.GetCamera(), player)) {
        ResetTool(); // Only reset if placement was successful
    }
}

void Engine::HandlePlayerRemoval() {
    if (IsMouseOverUI()) return;
    Vector2 mouseScreen = GetMousePosition();
    if (playerManager.TryRemovePlayer(mouseScreen, grid.GetCamera(), player)) {
        ResetTool(); // Only reset if removal was successful
    }
}
// =======================================
// =           Enemy Functions           =
// =======================================
void Engine::HandleEnemyPlacement() {
    if (IsMouseOverUI()) return;
    Vector2 mouseScreen = GetMousePosition();
    if (enemyManager.TryPlaceEnemy(mouseScreen, grid.GetCamera(), enemies)) {
        ResetTool(); // Only reset if placement was successful
    }
}
void Engine::HandleEnemyRemoval() {
    if (IsMouseOverUI()) return;
    Vector2 mouseScreen = GetMousePosition();
    if (enemyManager.TryRemoveEnemy(mouseScreen, grid.GetCamera(), enemies)) {
        ResetTool(); // Only reset if removal was successful
    }
}
// =======================================
void Engine::Run() {
    while (!WindowShouldClose()) {
        // Handle input based on mode and tool
        HandleEditModeInput();

        BeginDrawing();
        ClearBackground(GRAY);

        // Always update grid for rendering, but input is handled in HandleEditModeInput
        grid.Draw();

        // Draw camera selection area if selecting
        if (currentTool == ToolState::CAMERA_SELECTION && (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || selectedArea.width > 0)) {
            BeginMode2D(grid.GetCamera());
            DrawRectangleRec(selectedArea, Fade(BLUE, 0.3f));
            DrawRectangleLinesEx(selectedArea, 2.0f, BLUE);
            EndMode2D();
        }

        if (currentMode == Mode::EDIT) {
            // In edit mode, draw everything with the grid camera
            BeginMode2D(grid.GetCamera());
            if (player) {
                player->Draw();
            }
            for (auto& enemy : enemies) {
                enemy->Draw();
            }
            EndMode2D();
        }
        if (currentMode == Mode::PLAY) {
            // In play mode, draw everything with the player camera
            if (player) {
                player->Update(GetFrameTime());
                for (auto& enemy : enemies) {
                    enemy->Update(GetFrameTime(), player.get());
                }

                BeginMode2D(playerCamera);
                player->Draw();
                for (auto& enemy : enemies) {
                    enemy->Draw();
                }
                EndMode2D();
            }
        }

        rlImGuiBegin();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 4.0f, io.DisplaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Control panel", NULL,
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        grid.RenderSizeAvailability();
        RenderModeControls();
        RenderPlayerCameraSelectionUI();

        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }
}

void Engine::Shutdown() {
    CloseWindow();
    rlImGuiShutdown();
}