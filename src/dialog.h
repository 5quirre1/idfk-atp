#pragma once
#include "raylib.h"
#include "person.h"
#include "raymath.h"
#include <string>
#include <vector>
#include <algorithm>

// dialog settings
struct DialogSettings {
    float typewriterSpeed = 30.0f;
    float boxWidth = 600.0f;
    float boxHeight = 150.0f;
    float faceSize = 100.0f;
    float textPadding = 20.0f;
    Color boxColor = {0, 0, 0, 200};
    Color borderColor = WHITE;
    Color textColor = WHITE;
    float borderThickness = 3.0f;
    int fontSize = 24;
    float lineSpacing = 4.0f;
};

// dialog entry
struct DialogEntry {
    std::string text;
    Texture2D faceSprite;
    Sound speechSound;
    bool faceLoaded = false;
    bool soundLoaded = false;
};

// main system
struct DialogSystem {
    std::vector<DialogEntry> entries;
    DialogSettings settings;
    Font comicSansFont;
    Font comicSansBoldFont;
    bool fontLoaded = false;
    bool isActive = false;
    int currentEntry = 0;
    int currentChar = 0;
    float typewriterTimer = 0.0f;
    bool entryComplete = false;
    float lastSoundTime = 0.0f;
    const float soundInterval = 0.05f;
};

// init it
inline DialogSystem InitDialog() {
    DialogSystem dialog = {};
    
    dialog.comicSansFont = LoadFontEx("assets/comic.ttf", 24, 0, 250);
    if (dialog.comicSansFont.texture.id == 0) {
        dialog.comicSansFont = GetFontDefault();
        dialog.fontLoaded = false;
    } else {
        dialog.fontLoaded = true;
    }
    
    return dialog;
}

// add dialog entry
inline void AddDialogEntry(DialogSystem& dialog, const std::string& text, 
                          const char* faceSpritePath = nullptr, 
                          const char* speechSoundPath = nullptr) {
    DialogEntry entry;
    entry.text = text;
    
    if (faceSpritePath) {
        entry.faceSprite = LoadTexture(faceSpritePath);
        entry.faceLoaded = true;
    }
    
    if (speechSoundPath) {
        entry.speechSound = LoadSound(speechSoundPath);
        entry.soundLoaded = true;
    }
    
    dialog.entries.push_back(entry);
}

// start dialog
inline void StartDialog(DialogSystem& dialog) {
    if (dialog.entries.empty()) return;
    
    dialog.isActive = true;
    dialog.currentEntry = 0;
    dialog.currentChar = 0;
    dialog.typewriterTimer = 0.0f;
    dialog.entryComplete = false;
    dialog.lastSoundTime = 0.0f;
}

// update dialog system
inline void UpdateDialog(DialogSystem& dialog, float deltaTime) {
    if (!dialog.isActive || dialog.entries.empty()) return;
    
    DialogEntry& current = dialog.entries[dialog.currentEntry];
    
    if (!dialog.entryComplete) {
        dialog.typewriterTimer += deltaTime;
        
        float charTime = 1.0f / dialog.settings.typewriterSpeed;
        while (dialog.typewriterTimer >= charTime && dialog.currentChar < current.text.length()) {
            dialog.typewriterTimer -= charTime;
            dialog.currentChar++;
            if (current.soundLoaded && GetTime() - dialog.lastSoundTime > dialog.soundInterval) {
                PlaySound(current.speechSound);
                dialog.lastSoundTime = GetTime();
            }
        }

        if (dialog.currentChar >= current.text.length()) {
            dialog.entryComplete = true;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            dialog.currentChar = current.text.length();
            dialog.entryComplete = true;
        }
    } else {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            dialog.currentEntry++;
            
            if (dialog.currentEntry >= dialog.entries.size()) {
                dialog.isActive = false;
                dialog.currentEntry = 0;
            } else {
                dialog.currentChar = 0;
                dialog.typewriterTimer = 0.0f;
                dialog.entryComplete = false;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        dialog.isActive = false;
        dialog.currentEntry = 0;
    }
}

// draw dialog
inline void DrawDialog(const DialogSystem& dialog) {
    if (!dialog.isActive || dialog.entries.empty()) return;
    
    const DialogEntry& current = dialog.entries[dialog.currentEntry];
    const DialogSettings& settings = dialog.settings;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    float boxX = (screenWidth - settings.boxWidth) / 2.0f;
    float boxY = screenHeight - settings.boxHeight - 50.0f;

    DrawRectangleRounded({boxX, boxY, settings.boxWidth, settings.boxHeight}, 0.02f, 8, settings.boxColor);
    
    DrawRectangleRoundedLines({boxX, boxY, settings.boxWidth, settings.boxHeight}, 
                             0.02f, 8, settings.borderColor);
    
    float contentX = boxX + settings.textPadding;
    float contentY = boxY + settings.textPadding;
    
    if (current.faceLoaded) {
        Rectangle faceRect = {contentX, contentY, settings.faceSize, settings.faceSize};
        DrawTextureRec(current.faceSprite, 
                      {0, 0, (float)current.faceSprite.width, (float)current.faceSprite.height},
                      {faceRect.x, faceRect.y}, WHITE);
        contentX += settings.faceSize + settings.textPadding;
    }
    
    std::string displayText = current.text.substr(0, dialog.currentChar);
    
    float maxTextWidth = settings.boxWidth - (contentX - boxX) - settings.textPadding;
    Vector2 textSize = MeasureTextEx(dialog.comicSansFont, displayText.c_str(), settings.fontSize, 1.0f);
    
    DrawTextEx(dialog.comicSansFont, displayText.c_str(), 
              {contentX, contentY + 10}, settings.fontSize, 1.0f, settings.textColor);
    
    // continye
    if (dialog.entryComplete) {
        float triPadding = settings.textPadding;
        float triSize = 16.0f;
        float indicatorX = boxX + settings.boxWidth - triPadding - triSize - 4.0f;
        float indicatorY = boxY + settings.boxHeight - triPadding - triSize * 0.5f;

        Color triFill = settings.textColor;
        triFill.a = 255;

        Vector2 p1 = {indicatorX, indicatorY - triSize * 0.5f};
        Vector2 p2 = {indicatorX + triSize, indicatorY};
        Vector2 p3 = {indicatorX, indicatorY + triSize * 0.5f};

        DrawTriangle(p1, p2, p3, triFill);
        DrawTriangleLines(p1, p2, p3, settings.borderColor);
    }
}

// check IF player can interact with the guy
inline bool CanInteractWithPerson(Vector3 playerPos, Vector3 lookDir, const Person& person, 
                                 float maxDistance = 3.0f, float maxAngle = 0.8f) {
    // check distance
    float dx = person.position.x - playerPos.x;
    float dz = person.position.z - playerPos.z;
    float distance = sqrtf(dx*dx + dz*dz);
    
    if (distance > maxDistance) return false;
    
    // chrcking the angle (are we actualaly looking at the guy??)
    Vector3 toPersonDir = {dx / distance, 0.0f, dz / distance};
    Vector3 normalizedLookDir = {lookDir.x, 0.0f, lookDir.z};
    float len = sqrtf(normalizedLookDir.x*normalizedLookDir.x + normalizedLookDir.z*normalizedLookDir.z);
    if (len > 0) {
        normalizedLookDir.x /= len;
        normalizedLookDir.z /= len;
    }
    
    float dot = normalizedLookDir.x * toPersonDir.x + normalizedLookDir.z * toPersonDir.z;
    
    return dot > maxAngle;
}

// interactipon
inline void DrawInteractionIndicator() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f - 100.0f;
    
    // draw the e to talk thingie
    const char* text = "press 'e' to talk";
    Font font = GetFontDefault();
    int fontSize = 20;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.0f);
    
    float textX = centerX - textSize.x / 2.0f;
    float textY = centerY - textSize.y / 2.0f;
    
    // draw background
    DrawRectangleRounded({textX - 10, textY - 5, textSize.x + 20, textSize.y + 10}, 
                        0.3f, 8, {0, 0, 0, 150});
    
    // draw text
    DrawTextEx(font, text, {textX, textY}, fontSize, 1.0f, WHITE);
}

// unload dialog
inline void UnloadDialog(DialogSystem& dialog) {
    for (auto& entry : dialog.entries) {
        if (entry.faceLoaded) {
            UnloadTexture(entry.faceSprite);
        }
        if (entry.soundLoaded) {
            UnloadSound(entry.speechSound);
        }
    }
    
    if (dialog.fontLoaded) {
        UnloadFont(dialog.comicSansFont);
    }
    
    dialog.entries.clear();
}
