#include "Engine.h"

using namespace std;

Engine::Engine() { }

Engine::~Engine() { }

void Engine::Init() {
    InitWindow(800, 400, "nigga engine");

    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    rlImGuiSetup(true);
}

void Engine::HandlePlayerPlacement() {
    // Check for right mouse button click
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseScreen = GetMousePosition();
        Camera2D camera = grid.GetCamera();

        // Convert mouse position to world coordinates
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);

        // Calculate grid cell coordinates
        int tileSize = grid.GetTileSize();
        int gridX = static_cast<int>(mouseWorld.x / tileSize);
        int gridY = static_cast<int>(mouseWorld.y / tileSize);

        // Create or move the player
        if (!player) {
            player = std::make_unique<PlayerEntity>(grid, gridX, gridY);
        } else {
            player->PlaceOnGrid(gridX, gridY);
        }
    }
}

void Engine::Run() {
    while (!WindowShouldClose()) {
        // Handle input
        HandlePlayerPlacement();

        BeginDrawing();
        ClearBackground(GRAY);

        // Update and draw game objects
        grid.Update();
        grid.Draw();

        if (player) {
            player->Update(GetFrameTime());
            player->Draw();
        }

        // Draw UI
        rlImGuiBegin();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 4.0f, io.DisplaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Control panel", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        grid.RenderSizeAvailability();
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }
}

void Engine::Shutdown() {
    CloseWindow();
    rlImGuiShutdown();
}
