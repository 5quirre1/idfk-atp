// camera_bob.h
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "jump.h"
#include <cmath>

// settings for the camera bob
struct BobSettings
{
    float amplitude = 0.03f;
    float frequency = 8.0f;
    float sprintMultiplier = 1.6f;
    float idleAmplitude = 0.025f;
    float idleFrequency = 1.2f;
    float jumpBobAmplitude = 0.02f;
    float landBobAmount = 0.08f;
    float landRecoverySpeed = 8.0f;
};

// update the camera with bpbomng LMFAO
static void UpdateFirstPersonCameraWithBobbing(Camera3D &camera, Vector3 &position, Vector3 &target,
                                               const Vector3 forward, bool isMoving, bool isSprinting,
                                               float deltaTime, const JumpState& jump,
                                               BobSettings settings = BobSettings())
{
    float baseHeight = 1.6f;
    camera.position = position;
    camera.target = Vector3Add(position, target);

    static float bobTime = 0.0f;
    static float landBobTime = 0.0f;
    static bool wasGrounded = true;

    float bob = 0.0f;
    float landBob = 0.0f;

    if (jump.isGrounded) {
        if (isMoving) {
            bobTime += deltaTime * settings.frequency * (isSprinting ? settings.sprintMultiplier : 1.0f);
            bob = std::sin(bobTime) * settings.amplitude;
        } else {
            bobTime += deltaTime * settings.idleFrequency;
            bob = std::sin(bobTime) * settings.idleAmplitude;
        }
        
        if (!wasGrounded && jump.isGrounded) {
            landBobTime = 0.0f;
        }
    } else {
        bobTime += deltaTime * settings.idleFrequency * 0.5f;
        bob = std::sin(bobTime) * settings.jumpBobAmplitude;
    }
    
    if (jump.isGrounded && landBobTime < 1.0f) {
        landBobTime += deltaTime * settings.landRecoverySpeed;
        if (landBobTime > 1.0f) landBobTime = 1.0f;
        
        float landProgress = 1.0f - landBobTime;
        landProgress = landProgress * landProgress;
        landBob = -settings.landBobAmount * landProgress;
    }
    
    float totalBob = bob + landBob;
    
    camera.position.y = position.y + baseHeight + totalBob;
    camera.target.y = position.y + baseHeight + totalBob + target.y;
    
    wasGrounded = jump.isGrounded;
}
