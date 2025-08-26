#pragma once
#include "raylib.h"

// walk slash run stuff 
struct WalkState {
    Sound step1;
    Sound step2;
    bool nextStepFirst;
    float stepInterval;
    float stepTimer;
};

// init walk sound
void InitWalk(WalkState &walk) {
    walk.step1 = LoadSound("assets/step1.wav");
    walk.step2 = LoadSound("assets/step2.wav");
    walk.nextStepFirst = true;
    walk.stepInterval = 0.45f;
    walk.stepTimer = 0.0f;
}

// unload walk
void UnloadWalk(WalkState &walk) {
    UnloadSound(walk.step1);
    UnloadSound(walk.step2);
}

// update walk stuff
void UpdateWalk(WalkState &walk, bool isMoving, bool isSprinting, float deltaTime) {
    if (isMoving) {
        float interval = walk.stepInterval / (isSprinting ? 1.5f : 1.0f);
        walk.stepTimer += deltaTime;

        if (walk.stepTimer >= interval) {
            walk.stepTimer = 0.0f;
            if (walk.nextStepFirst) {
                PlaySound(walk.step1);
            } else {
                PlaySound(walk.step2);
            }
            walk.nextStepFirst = !walk.nextStepFirst;
        }
    } else {
        walk.stepTimer = 0.0f; // reset when not moving
    }
}
