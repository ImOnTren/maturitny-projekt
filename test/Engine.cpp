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


void Engine::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        rlImGuiBegin();

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 4.0f, io.DisplaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Control panel", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        grid.RenderSizeAvailability();

        ImGui::End();
        rlImGuiEnd();

        grid.Update();
        grid.Draw();

        EndDrawing();
    }
}

void Engine::Shutdown() {
    CloseWindow();
    rlImGuiShutdown();
}
