#pragma once
#include "raylib.h"

// jump variables
struct JumpState {
    float verticalVelocity = 0.0f;
    bool isGrounded = true;
    bool isJumping = false;
    float groundLevel = 0.0f;
    float jumpForce = 8.0f;
    float gravity = 20.0f;
    float maxFallSpeed = 15.0f;
    float coyoteTime = 0.1f;
    float jumpBufferTime = 0.1f;
    float timeSinceGrounded = 0.0f;
    float timeSinceJumpPressed = 999.0f;
    Sound jumpSound;
    bool soundLoaded = false;
};

// init jump
inline void InitJump(JumpState& jump) {
    jump.verticalVelocity = 0.0f;
    jump.isGrounded = true;
    jump.isJumping = false;
    jump.groundLevel = 0.0f;
    jump.timeSinceGrounded = 0.0f;
    jump.timeSinceJumpPressed = 999.0f;
    jump.jumpSound = LoadSound("assets/core/jump.mp3");
    jump.soundLoaded = true;
}

// unload jump assets
inline void UnloadJump(JumpState& jump) {
    if (jump.soundLoaded) {
        UnloadSound(jump.jumpSound);
        jump.soundLoaded = false;
    }
}

// update jump
inline void UpdateJump(JumpState& jump, Vector3& playerPos, float deltaTime) {
    jump.timeSinceGrounded += deltaTime;
    jump.timeSinceJumpPressed += deltaTime;
    
    if (IsKeyPressed(KEY_SPACE)) {
        jump.timeSinceJumpPressed = 0.0f;
    }
    
    bool wasGrounded = jump.isGrounded;
    jump.isGrounded = (playerPos.y <= jump.groundLevel);
    
    if (jump.isGrounded) {
        jump.timeSinceGrounded = 0.0f;
        
        if (!wasGrounded && jump.verticalVelocity < 0) {
            jump.isJumping = false;
            jump.verticalVelocity = 0.0f;
            playerPos.y = jump.groundLevel;
        }
    
        if (jump.timeSinceJumpPressed <= jump.jumpBufferTime && 
            jump.timeSinceGrounded <= jump.coyoteTime && 
            !jump.isJumping) {
            
            jump.verticalVelocity = jump.jumpForce;
            jump.isJumping = true;
            jump.isGrounded = false;
            jump.timeSinceJumpPressed = 999.0f;
            // play jim[]
            if (jump.soundLoaded) {
                PlaySound(jump.jumpSound);
            }
        }
    } else {
        jump.verticalVelocity -= jump.gravity * deltaTime;
        if (jump.verticalVelocity < -jump.maxFallSpeed) {
            jump.verticalVelocity = -jump.maxFallSpeed;
        }
    }
    
    playerPos.y += jump.verticalVelocity * deltaTime;
    
    if (playerPos.y < jump.groundLevel) {
        playerPos.y = jump.groundLevel;
        jump.verticalVelocity = 0.0f;
        jump.isGrounded = true;
        jump.isJumping = false;
    }
}

// check if player can even jump
inline bool CanJump(const JumpState& jump) {
    return jump.timeSinceGrounded <= jump.coyoteTime && !jump.isJumping;
}

// get jump height percentage
inline float GetJumpHeightPercentage(const JumpState& jump) {
    if (jump.isGrounded || !jump.isJumping) return 0.0f;
    
    float maxVelocity = jump.jumpForce;
    if (jump.verticalVelocity > 0) {
        return 1.0f - (jump.verticalVelocity / maxVelocity);
    }
    return 1.0f;
}
