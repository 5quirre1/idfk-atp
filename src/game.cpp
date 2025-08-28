// i'm trying out commentibg on my code to try to make it understandabnle ig?? so uh sorry if no understand
#include "raylib.h"
#include "camera_bob.h"
#include "sky.h"
#include "walk.h"
#include "person.h"
#include "game.h"
#include "dialog.h"
#include "jump.h"
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

    // more camera sutff:brokneheart
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

    Texture2D groundTex = LoadTexture("assets/core/dog.png");

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

    // jump system
    JumpState jump;
    InitJump(jump);
    jump.groundLevel = 0.0f;  // set ground level

    // person
    Person swagGuy = LoadPerson("assets/core/swag-guy.png", {2.0f, 1.0f, 0.0f}, 2.0f);

    // skybox
    Sky sky = LoadSky("assets/core/skybox.png");

    // dialog system setup
    DialogSystem dialog = InitDialog();
    bool canInteract = false;

    // add dialog entries
    AddDialogEntry(dialog, "hi my name is swag guy", "assets/core/swag-guy-face.png", "assets/core/dialog-swag.wav");
    AddDialogEntry(dialog, "i work at mcdonalds n stuff", "assets/core/swag-guy-face.png", "assets/core/dialog-swag.wav");
    AddDialogEntry(dialog, "great food too btw...", "assets/core/swag-guy-face.png", "assets/core/dialog-swag.wav");
    AddDialogEntry(dialog, "oh yea, use ur mouse to look\naround and use WASD to move", "assets/core/swag-guy-face.png", "assets/core/dialog-swag.wav");
    AddDialogEntry(dialog, "oh and shift to yk run LMFAO", "assets/core/swag-guy-face.png", "assets/core/dialog-swag.wav");
    AddDialogEntry(dialog, "oh and SPACE to jump btw!", "assets/core/swag-guy-face.png", "assets/core/dialog-swag.wav");

    // define world borders
    const float worldBorderX = 20.0f;
    const float worldBorderZ = 20.0f;

    // main loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!dialog.isActive) {
            // mouse look (only when dialog is not active)
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
                Vector3 newPlayerPos = Vector3Add(playerPos, Vector3Scale(moveDelta, speed * dt));

                // check before moving again
                if (!CheckPersonCollision(newPlayerPos, swagGuy)) {
                    playerPos.x = newPlayerPos.x;
                    playerPos.z = newPlayerPos.z;
                }

                // apply world borders after updating playerPos
                if (playerPos.x > worldBorderX) playerPos.x = worldBorderX;
                if (playerPos.x < -worldBorderX) playerPos.x = -worldBorderX;
                if (playerPos.z > worldBorderZ) playerPos.z = worldBorderZ;
                if (playerPos.z < -worldBorderZ) playerPos.z = -worldBorderZ;
            }

            // update jump physics
            UpdateJump(jump, playerPos, dt);

            // look direction
            Vector3 lookDir = { cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw) };

            // camera & footsteps (only when dialog is not active)
            UpdateFirstPersonCameraWithBobbing(camera, playerPos, lookDir, forward, moving && jump.isGrounded, sprint, dt, jump, bobSettings);
            UpdateWalk(walk, moving && jump.isGrounded, sprint, dt);

            // check interaction with person
            canInteract = CanInteractWithPerson(playerPos, lookDir, swagGuy);

            // start dialog when pressing E
            if (canInteract && IsKeyPressed(KEY_E)) {
                StartDialog(dialog);
            }
        } else {
            // no jump
            jump.verticalVelocity = 0.0f;
            if (playerPos.y < jump.groundLevel) {
                playerPos.y = jump.groundLevel;
                jump.isGrounded = true;
                jump.isJumping = false;
            }
        }


        // update dialog system
        UpdateDialog(dialog, dt);

        // render
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawSky(sky, camera);
                DrawModel(floorModel, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
                DrawPerson(swagGuy, camera);
            EndMode3D();

            // draw interaction indicator
            if (canInteract && !dialog.isActive) {
                DrawInteractionIndicator();
            }

            // draw dialog
            DrawDialog(dialog);

        EndDrawing();
    }

    // cleanup
    UnloadJump(jump);
    UnloadDialog(dialog);
    UnloadWalk(walk);
    UnloadModel(floorModel);
    UnloadTexture(groundTex);
    UnloadPerson(swagGuy);
    UnloadSky(sky);
}
