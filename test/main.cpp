#include "Engine.h"

Vector3 Vec3(float x, float y, float z) {
    Vector3 v = { x, y, z };
    return v;
}
Vector2 Vec2(float x, float y) {
    Vector2 v = { x, y };
    return v;
}

int main(){

    Engine Engine;
    Engine.Init();
    Engine.Run();
    Engine.Shutdown();

    return 0;
}