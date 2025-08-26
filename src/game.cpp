// i'm trying out commentibg on my code to try to make it understandabnle ig?? so uh sorry if no understand
#include "raylib.h"
#include "camera_bob.h"
#include "sky.h"
#include "walk.h"
#include "person.h"
#include "game.h"
#include <cmath>

void RunGame() {
    // camera setup
    Camera3D camera = {
        .position = {0, 0, 0},
        .target = {0, 0, 0},
        .up = {0, 1, 0},
        .fovy = 60.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    
    // more camera sutff:brokne
    camera.position = (Vector3){ 0.0f, 1.6f, 4.0f };
    camera.target   = (Vector3){ 0.0f, 1.6f, 0.0f };
    camera.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy     = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 playerPos = { 0.0f, 0.0f, 0.0f };
    float yaw = 0.0f;
    float pitch = 0.0f;

    const float moveSpeed = 3.5f;
    const float mouseSensitivity = 0.0025f;

    Texture2D groundTex = LoadTexture("assets/dog.png");

    Mesh floorMesh = GenMeshPlane(40.0f, 40.0f, 20, 20);
    Model floorModel = LoadModelFromMesh(floorMesh);
    Material floorMat = floorModel.materials[0];
    floorMat.maps[MATERIAL_MAP_DIFFUSE].texture = groundTex;
    floorModel.materials[0] = floorMat;

    DisableCursor();

    // camera bob
    BobSettings bobSettings;
    bobSettings.amplitude = 0.035f;
    bobSettings.frequency = 9.0f;

    // footsteps
    WalkState walk;
    InitWalk(walk);

    // person
    Person swagGuy = LoadPerson("assets/swag-guy.png", {2.0f, 1.0f, 0.0f}, 2.0f);

    // skybox
    Sky sky = LoadSky("assets/skybox.png");

    // main loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // mouse look
        Vector2 mouseDelta = GetMouseDelta();
        yaw += -mouseDelta.x * mouseSensitivity;
        pitch += -mouseDelta.y * mouseSensitivity;
        if (pitch >  1.45f) pitch =  1.45f;
        if (pitch < -1.45f) pitch = -1.45f;

        // movement dirs
        Vector3 forward = { sinf(yaw), 0.0f, cosf(yaw) };
        Vector3 right   = { -forward.z, 0.0f, forward.x };

        bool moving = false;
        bool sprint = IsKeyDown(KEY_LEFT_SHIFT);
        float speed = moveSpeed * (sprint ? 1.8f : 1.0f);

        Vector3 moveDelta = { 0, 0, 0 };
        if (IsKeyDown(KEY_W)) { moveDelta = Vector3Add(moveDelta, forward); moving = true; }
        if (IsKeyDown(KEY_S)) { moveDelta = Vector3Subtract(moveDelta, forward); moving = true; }
        if (IsKeyDown(KEY_A)) { moveDelta = Vector3Subtract(moveDelta, right); moving = true; }
        if (IsKeyDown(KEY_D)) { moveDelta = Vector3Add(moveDelta, right); moving = true; }

        // normalize movement
        if (moving) {
            float len = sqrtf(moveDelta.x*moveDelta.x + moveDelta.y*moveDelta.y + moveDelta.z*moveDelta.z);
            if (len != 0.0f) {
                moveDelta.x /= len; moveDelta.y /= len; moveDelta.z /= len;
            }
            playerPos = Vector3Add(playerPos, Vector3Scale(moveDelta, speed * dt));
        }

        // look direction
        Vector3 lookDir = { cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw) };

        // camera & footsteps
        UpdateFirstPersonCameraWithBobbing(camera, playerPos, lookDir, forward, moving, sprint, dt, bobSettings);
        UpdateWalk(walk, moving, sprint, dt);

        // render
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawSky(sky, camera);
                DrawModel(floorModel, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
                DrawPerson(swagGuy, camera);
            EndMode3D();
        EndDrawing();
    }

    // cleanup
    UnloadWalk(walk);
    UnloadModel(floorModel);
    UnloadTexture(groundTex);
    UnloadPerson(swagGuy);
    UnloadSky(sky);
}
