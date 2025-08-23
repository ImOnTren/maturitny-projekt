#include "Engine.h"

using namespace std;

Engine::Engine() {
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
        case ToolState::PLACING:
            HandlePlayerPlacement();
            break;
        case ToolState::REMOVING:
            HandlePlayerRemoval();
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

void Engine::ConfirmPlayerCameraSelection() {
    // Set up player camera to focus on the selected area
    playerCamera.target = {
        selectedArea.x + selectedArea.width / 2,
        selectedArea.y + selectedArea.height / 2
    };

    // Adjust zoom based on the selected area size (optional)
    // For now, keep zoom at 1.0f

    TraceLog(LOG_INFO, "Player camera set to area: (%.0f, %.0f, %.0f, %.0f)",
             selectedArea.x, selectedArea.y, selectedArea.width, selectedArea.height);
}

void Engine::CancelPlayerCameraSelection() {
    // Reset selection to x = 0 y = 0 width = 0 height = 0
    selectionStart = {0, 0};
    selectionEnd = {0, 0};
    selectedArea = {0, 0, 0, 0};
}

void Engine::RenderModeControls() {
    ImGui::Separator();
    ImGui::Text("Engine Mode:");

    if (ImGui::RadioButton("Edit Mode", currentMode == Mode::EDIT)) {
        currentMode = Mode::EDIT;
        currentTool = ToolState::NONE; // Reset tool when switching modes
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Play Mode", currentMode == Mode::PLAY)) {
        currentMode = Mode::PLAY;
        currentTool = ToolState::NONE;
    }

    if (currentMode == Mode::EDIT) {
        ImGui::Separator();
        ImGui::Text("Edit Tools:");

        if (ImGui::RadioButton("Place Player", currentTool == ToolState::PLACING)) {
            currentTool = ToolState::PLACING;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Remove Player", currentTool == ToolState::REMOVING)) {
            currentTool = ToolState::REMOVING;
        }

        if (ImGui::Button("Set Player Camera Area")) {
            currentTool = ToolState::CAMERA_SELECTION;
            ImGui::SetTooltip("Click and drag on grid to select camera area");
        }

        // Show current tool status
        ImGui::Text("Current Tool: ");
        ImGui::SameLine();
        switch (currentTool) {
            case ToolState::NONE: ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "None"); break;
            case ToolState::PLACING: ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Placing Player"); break;
            case ToolState::REMOVING: ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Removing Player"); break;
            case ToolState::CAMERA_SELECTION: ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "Selecting Camera Area"); break;
        }
    }
}

void Engine::HandlePlayerPlacement() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseScreen = GetMousePosition();
        Camera2D camera = grid.GetCamera();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);

        int tileSize = grid.GetTileSize();
        int gridX = static_cast<int>(mouseWorld.x / tileSize);
        int gridY = static_cast<int>(mouseWorld.y / tileSize);

        if (!player) {
            player = std::make_unique<PlayerEntity>(grid, gridX, gridY);
        }
        // If player already exists, update its position (Not aure if this is needed)
        //else {
        //    player->PlaceOnGrid(gridX, gridY);
        //}
    }
}

void Engine::HandlePlayerRemoval() {

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && player) {
        Vector2 mouseScreen = GetMousePosition();
        Camera2D camera = grid.GetCamera();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);

        int tileSize = grid.GetTileSize();
        int clickedGridX = static_cast<int>(mouseWorld.x / tileSize);
        int clickedGridY = static_cast<int>(mouseWorld.y / tileSize);

        int playerGridX = player->GetGridX();
        int playerGridY = player->GetGridY();

        if (clickedGridX == playerGridX && clickedGridY == playerGridY) {
            player.reset();
        }
    }
}

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

        if (player) {
            if (currentMode == Mode::PLAY) {
                player->Update(GetFrameTime());
            }
            player->Draw();
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