#pragma once
#include "raylib.h"

// sky
struct Sky {
    Model model;
    Texture2D texture;
};

// load the sky
inline Sky LoadSky(const char* filename) {
    Sky sky{};

    // generate the sphere
    Mesh sphere = GenMeshSphere(1.0f, 32, 32);
    sky.model = LoadModelFromMesh(sphere);

    // load the texture
    sky.texture = LoadTexture(filename);

    // use texture
    sky.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sky.texture;

    return sky;
}

// unload assets
inline void UnloadSky(Sky& sky) {
    UnloadTexture(sky.texture);
    UnloadModel(sky.model);
}

// draw the sky
inline void DrawSky(const Sky& sky, const Camera3D& camera) {
    Vector3 pos = camera.position;
    DrawModelEx(sky.model, pos, (Vector3){1,0,0}, 0.0f, (Vector3){-50.0f, 50.0f, 50.0f}, WHITE);
}
