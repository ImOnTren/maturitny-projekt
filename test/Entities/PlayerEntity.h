#pragma once
#include "Entity.h"
#include "Grid.h"

class PlayerEntity : public Entity {
public:
    PlayerEntity(Grid& grid, int gridX = 0, int gridY = 0);
    ~PlayerEntity();
    void Update(float deltaTime) override;
    void Draw() override;
    void PlaceOnGrid(int gridX, int gridY);

    int GetGridX() const { return cellX; }
    int GetGridY() const { return cellY; }

    bool LoadPlayerTexture(const char* path);
private:
    Grid& grid;
    int cellX, cellY;
    Texture2D playerTexture;
    bool textureLoaded = false;
};