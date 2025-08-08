#include "Grid.h"

Grid::Grid() {
    camera.offset = { 0, 0 };
    camera.target = { 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = zoom;
}

Grid::~Grid() {
    if (initialized) UnloadRenderTexture(gridTexture);
}

void Grid::RenderSizeAvailability() {
    for (int i = 0; i < tileSize.size(); i++) {
        if (ImGui::RadioButton(tileSize[i].c_str(), selectedTileSizeIndex == i)) {
            selectedTileSizeIndex = i;
            needsRedraw = true;
        }
    }
}

void Grid::Update() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float panelWidth = screenWidth / 4.0f;
    camera.offset = { panelWidth, 0 };

    if (!initialized) {
        gridTexture = LoadRenderTexture(screenWidth, screenHeight);
        initialized = true;
        needsRedraw = true;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouseScreen = GetMousePosition();
        Vector2 worldBeforeZoom = GetScreenToWorld2D(mouseScreen, camera);

        zoom += wheel * 0.1f;
        if (zoom < 0.1f) zoom = 0.1f;
        if (zoom > 4.0f) zoom = 4.0f;
        camera.zoom = zoom;

        Vector2 worldAfterZoom = GetScreenToWorld2D(mouseScreen, camera);
        Vector2 offset = Vector2Subtract(worldBeforeZoom, worldAfterZoom);
        camera.target = Vector2Add(camera.target, offset);

        needsRedraw = true;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
        needsRedraw = true;
    }
}

void Grid::Draw() {
    int tileSize = GetTileSize();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float panelWidth = screenWidth / 4.0f;

    if (needsRedraw) {
        BeginTextureMode(gridTexture);
        ClearBackground(GRAY);

        BeginMode2D(camera);

        Vector2 topLeft = GetScreenToWorld2D({ panelWidth, 0 }, camera);
        Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

        int startX = (int)std::ceil(topLeft.x / tileSize);
        int endX = (int)(bottomRight.x / tileSize);
        int startY = (int)(topLeft.y / tileSize);
        int endY = (int)(bottomRight.y / tileSize);

        if (startX < 0) startX = 0;
        if (startY < 0) startY = 0;

        for (int y = startY; y <= endY; y++) {
            for (int x = startX; x <= endX; x++) {
                DrawRectangleLines(x * tileSize, y * tileSize, tileSize, tileSize, DARKGRAY);
            }
        }

        EndMode2D();
        EndTextureMode();

        needsRedraw = false;
    }

    Rectangle sourceRect = {
        0.0f,
        0.0f,
        (float)gridTexture.texture.width,
        -(float)gridTexture.texture.height // Negative to flip vertically
    };

    Vector2 position = { 0.0f, 0.0f };

    DrawTextureRec(gridTexture.texture, sourceRect, position, WHITE);
    

}