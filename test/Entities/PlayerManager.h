// PlayerManager.h
#pragma once
#include <memory>
#include "raylib.h"

class Grid;
class PlayerEntity;

class PlayerManager {
public:
    PlayerManager(Grid& grid); // Remove camera parameter

    bool TryPlacePlayer(Vector2 mouseScreen, Camera2D camera, std::unique_ptr<PlayerEntity>& player);
    bool TryRemovePlayer(Vector2 mouseScreen, Camera2D camera, std::unique_ptr<PlayerEntity>& player);

private:
    Grid& grid;
};