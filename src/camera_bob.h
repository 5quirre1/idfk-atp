#pragma once

#include "raylib.h"
#include "raymath.h"
#include <cmath>

// settings for the camera bob
struct BobSettings
{
    float amplitude = 0.03f;
    float frequency = 8.0f;
    float sprintMultiplier = 1.6f;
    float idleAmplitude = 0.025f;
    float idleFrequency = 1.2f;
};


// update the camera with bpbomng LMFAO
static void UpdateFirstPersonCameraWithBobbing(Camera3D &camera, Vector3 &position, Vector3 &target,
                                               const Vector3, bool isMoving, bool isSprinting,
                                               float deltaTime, BobSettings settings = BobSettings())
{
    float baseHeight = 1.6f;
    camera.position = position;
    camera.target = Vector3Add(position, target);

    static float bobTime = 0.0f;

    float bob = 0.0f;

    if (isMoving)
    {
        bobTime += deltaTime * settings.frequency * (isSprinting ? settings.sprintMultiplier : 1.0f);
        bob = std::sin(bobTime) * settings.amplitude;
    }
    else
    {
        bobTime += deltaTime * settings.idleFrequency;
        bob = std::sin(bobTime) * settings.idleAmplitude;
    }

    camera.position.y = position.y + baseHeight + bob;
    camera.target.y = position.y + baseHeight + bob + target.y;
}
