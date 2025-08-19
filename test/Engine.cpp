#include "Engine.h"

using namespace std;

Engine::Engine() { }

Engine::~Engine() { }

void Engine::Init() {
    InitWindow(800, 400, "Game Engine");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    rlImGuiSetup(true);
}

void Engine::RenderModeControls() {
    ImGui::Separator();
    ImGui::Text("Engine Mode:");
    
    if (ImGui::RadioButton("Edit Mode", currentMode == Mode::EDIT)) {
        currentMode = Mode::EDIT;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Play Mode", currentMode == Mode::PLAY)) {
        currentMode = Mode::PLAY;
    }
}

void Engine::HandlePlayerPlacement() {
    // Only allow placement in Edit mode
    if (currentMode != Mode::EDIT) return;
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseScreen = GetMousePosition();
        Camera2D camera = grid.GetCamera();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
        
        int tileSize = grid.GetTileSize();
        int gridX = static_cast<int>(mouseWorld.x / tileSize);
        int gridY = static_cast<int>(mouseWorld.y / tileSize);
        
        if (!player) {
            player = std::make_unique<PlayerEntity>(grid, gridX, gridY);
        } else {
            player->PlaceOnGrid(gridX, gridY);
        }
    }
}

void Engine::HandlePlayerRemoval() {
    // Only allow removal in Edit mode
    if (currentMode != Mode::EDIT) return;
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player) {
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
        // Handle input based on mode
        HandlePlayerPlacement();
        HandlePlayerRemoval();
        
        BeginDrawing();
        ClearBackground(GRAY);
        
        // Update game objects based on mode
        grid.Update();
        grid.Draw();
        
        if (player) {
            // Only update player movement in PLAY mode
            if (currentMode == Mode::PLAY) {
                player->Update(GetFrameTime());
            }
            player->Draw();
        }
        
        // Draw UI
        rlImGuiBegin();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 4.0f, io.DisplaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Control panel", NULL, 
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        grid.RenderSizeAvailability();
        RenderModeControls();
        
        ImGui::End();
        rlImGuiEnd();
        
        EndDrawing();
    }
}

void Engine::Shutdown() {
    CloseWindow();
    rlImGuiShutdown();
}