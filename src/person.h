#pragma once
#include "raylib.h"

// the person suff
struct Person {
    Texture2D texture;
    Vector3 position;
    float scale;
};

// loadming the person
inline Person LoadPerson(const char* filename, Vector3 pos, float scale = 1.0f) {
    Person p{};
    p.texture = LoadTexture(filename);
    p.position = pos;
    p.scale = scale;
    return p;
}

// unload person
inline void UnloadPerson(Person& p) {
    UnloadTexture(p.texture);
}

// draw person
inline void DrawPerson(const Person& p, const Camera3D& camera) {
    DrawBillboard(camera, p.texture, p.position, p.scale, WHITE); // raylib is so swag that they already have smth like ts :mending-heart:
}

// checks the collision to maek sure no going though the person
inline bool CheckPersonCollision(Vector3 playerPos, const Person& person, float playerRadius = 0.1f) {
    float dx = playerPos.x - person.position.x;
    float dz = playerPos.z - person.position.z;
    float distance = sqrtf(dx*dx + dz*dz);
    float personRadius = person.scale * 0.1f;
    
    return distance < (playerRadius + personRadius);
}
