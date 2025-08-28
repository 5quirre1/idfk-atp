#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_GAME,
    MENU_STATE_EXIT
} MenuState;

typedef struct {
    Rectangle rect;
    bool isHovered;
    bool isPressed;
    float hoverTime;
    Color baseColor;
    Color hoverColor;
    Color textColor;
    const char* text;
    Sound hoverSound;
    Sound clickSound;
} MenuButton;

typedef struct {
    Texture2D background;
    MenuButton playButton;
    MenuButton exitButton;
    MenuState currentState;
    Font titleFont;
    Font buttonFont;
    float fadeAlpha;
    bool initialized;
} Menu;

// all da functions
Menu InitMenu(void);
void UnloadMenu(Menu* menu);
MenuState UpdateMenu(Menu* menu);
void DrawMenu(Menu* menu);
void UpdateMenuButton(MenuButton* button, Vector2 mousePos, bool mousePressed);
void DrawMenuButton(MenuButton button, Font font);

#endif
