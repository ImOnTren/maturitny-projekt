#include "Grid.h"

Grid::Grid(){
    camera.offset = { 0, 0 };
    camera.target = { 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = zoom;
}

Grid::~Grid(){
    
}

void Grid::RenderSizeAvailability() {
    for (int i = 0; i < tileSize.size(); i++) {
        if (ImGui::RadioButton(tileSize[i].c_str(), selectedTileSizeIndex == i)) {
            selectedTileSizeIndex = i;
        }
    }
}

void Grid::Update() {
    float wheel = GetMouseWheelMove();

    if (wheel != 0) {
        zoom += wheel * 0.1f;
        if (zoom < 0.1f) zoom = 0.1f;
        if (zoom > 4.0f) zoom = 4.0f;
        camera.zoom = zoom;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();
        camera.target.x -= delta.x / zoom;
        camera.target.y -= delta.y / zoom;
    }
}

void Grid::Draw() {
    int tileSize = GetTileSize();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    float panelWidth = screenWidth / 4.0f;
    camera.offset = { panelWidth, 0 };

    BeginMode2D(camera);

    // Get world-space bounds of what’s currently visible
    Vector2 topLeft = GetScreenToWorld2D({ panelWidth, 0 }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

    // Align to tile grid
    int startX = (int)(topLeft.x / tileSize) - 1;
    int endX = (int)(bottomRight.x / tileSize) + 1;
    int startY = (int)(topLeft.y / tileSize) - 1;
    int endY = (int)(bottomRight.y / tileSize) + 1;

    // Clamp to 0 if you want a grid with boundaries
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            DrawRectangleLines(x * tileSize, y * tileSize, tileSize, tileSize, DARKGRAY);
        }
    }

    EndMode2D();
}


