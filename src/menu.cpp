#include "menu.h"
#include <math.h>

// init menu
Menu InitMenu(void) {
    Menu menu = {0};
    
    // load all da stuff
    menu.background = LoadTexture("assets/menu-bg.png");
    menu.titleFont = LoadFontEx("assets/core/comic.ttf", 72, 0, 250);
    if (menu.titleFont.texture.id == 0) {
        menu.titleFont = GetFontDefault();
    }
    
    menu.buttonFont = LoadFontEx("assets/core/comic.ttf", 32, 0, 250);
    if (menu.buttonFont.texture.id == 0) {
        menu.buttonFont = GetFontDefault();
    }
    
    menu.playButton.hoverSound = LoadSound("assets/core/menu/hover.wav");
    menu.playButton.clickSound = LoadSound("assets/core/menu/click.wav");
    menu.exitButton.hoverSound = LoadSound("assets/core/menu/hover.wav");
    menu.exitButton.clickSound = LoadSound("assets/core/menu/click.wav");
    
    // setup play button
    menu.playButton.rect = (Rectangle){
        GetScreenWidth() / 2.0f - 120,
        GetScreenHeight() / 2.0f - 20,
        240,
        60
    };
    menu.playButton.baseColor = (Color){45, 45, 55, 200};
    menu.playButton.hoverColor = (Color){65, 65, 85, 220};
    menu.playButton.textColor = (Color){220, 220, 230, 255};
    menu.playButton.text = "play";
    menu.playButton.isHovered = false;
    menu.playButton.isPressed = false;
    menu.playButton.hoverTime = 0.0f;
    
    // setup exit button
    menu.exitButton.rect = (Rectangle){
        GetScreenWidth() / 2.0f - 120,
        GetScreenHeight() / 2.0f + 60,
        240,
        60
    };
    menu.exitButton.baseColor = (Color){55, 35, 35, 200};
    menu.exitButton.hoverColor = (Color){85, 55, 55, 220};
    menu.exitButton.textColor = (Color){220, 200, 200, 255};
    menu.exitButton.text = "exit";
    menu.exitButton.isHovered = false;
    menu.exitButton.isPressed = false;
    menu.exitButton.hoverTime = 0.0f;
    
    menu.currentState = MENU_STATE_MAIN;
    menu.fadeAlpha = 1.0f;
    menu.initialized = true;
    
    return menu;
}

// unload menu assets
void UnloadMenu(Menu* menu) {
    if (!menu->initialized) return;
    
    UnloadTexture(menu->background);
    if (menu->titleFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(menu->titleFont);
    }
    if (menu->buttonFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(menu->buttonFont);
    }
    UnloadSound(menu->playButton.hoverSound);
    UnloadSound(menu->playButton.clickSound);
    UnloadSound(menu->exitButton.hoverSound);
    UnloadSound(menu->exitButton.clickSound);
    
    menu->initialized = false;
}

// update menu
MenuState UpdateMenu(Menu* menu) {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    UpdateMenuButton(&menu->playButton, mousePos, mousePressed);
    UpdateMenuButton(&menu->exitButton, mousePos, mousePressed);
    
    // check the bvitton clicks
    if (menu->playButton.isPressed) {
        menu->currentState = MENU_STATE_GAME;
        return MENU_STATE_GAME;
    }
    
    if (menu->exitButton.isPressed) {
        menu->currentState = MENU_STATE_EXIT;
        return MENU_STATE_EXIT;
    }
    
    return MENU_STATE_MAIN;
}

// update menu button
void UpdateMenuButton(MenuButton* button, Vector2 mousePos, bool mousePressed) {
    bool wasHovered = button->isHovered;
    button->isHovered = CheckCollisionPointRec(mousePos, button->rect);
    button->isPressed = false;
    
    // play hover sound when yk
    if (button->isHovered && !wasHovered) {
        PlaySound(button->hoverSound);
    }
    
    if (button->isHovered) {
        button->hoverTime += GetFrameTime();
        if (mousePressed) {
            button->isPressed = true;
            PlaySound(button->clickSound);
        }
    } else {
        button->hoverTime = 0.0f;
    }
}

// draw menhu
void DrawMenu(Menu* menu) {
    // draw backgroiynd
    if (menu->background.id > 0) {
        float scaleX = (float)GetScreenWidth() / menu->background.width;
        float scaleY = (float)GetScreenHeight() / menu->background.height;
        float scale = fmaxf(scaleX, scaleY);
        
        DrawTextureEx(menu->background, 
            (Vector2){
                (GetScreenWidth() - menu->background.width * scale) / 2,
                (GetScreenHeight() - menu->background.height * scale) / 2
            }, 
            0.0f, scale, WHITE);
    }
    // just a dark overlay to not destory people's eyes with bright pic
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 120});
    
    const char* title = "idfk";
    Vector2 titleSize = MeasureTextEx(menu->titleFont, title, 72, 2);
    Vector2 titlePos = (Vector2){
        (GetScreenWidth() - titleSize.x) / 2,
        GetScreenHeight() / 2 - 180
    };
    
    // main title
    DrawTextEx(menu->titleFont, title, titlePos, 72, 2, (Color){220, 240, 255, 255});
    
    // draw all buttons
    DrawMenuButton(menu->playButton, menu->buttonFont);
    DrawMenuButton(menu->exitButton, menu->buttonFont);
}

// draw menu button
void DrawMenuButton(MenuButton button, Font font) {
    Color currentColor = button.baseColor;
    float pulseEffect = 1.0f;
    
    if (button.isHovered) {
        float t = fminf(button.hoverTime * 3.0f, 1.0f);
        currentColor.r = (unsigned char)(button.baseColor.r + (button.hoverColor.r - button.baseColor.r) * t);
        currentColor.g = (unsigned char)(button.baseColor.g + (button.hoverColor.g - button.baseColor.g) * t);
        currentColor.b = (unsigned char)(button.baseColor.b + (button.hoverColor.b - button.baseColor.b) * t);
        currentColor.a = (unsigned char)(button.baseColor.a + (button.hoverColor.a - button.baseColor.a) * t);
        
        pulseEffect = 1.0f + sinf(button.hoverTime * 8.0f) * 0.05f;
    }
    
    Rectangle drawRect = button.rect;
    if (button.isHovered) {
        float scale = 1.0f + (pulseEffect - 1.0f) * 2.0f;
        float deltaW = drawRect.width * (scale - 1.0f) / 2.0f;
        float deltaH = drawRect.height * (scale - 1.0f) / 2.0f;
        drawRect.x -= deltaW;
        drawRect.y -= deltaH;
        drawRect.width += deltaW * 2.0f;
        drawRect.height += deltaH * 2.0f;
    }
    
    DrawRectangleRounded((Rectangle){
        drawRect.x + 4, drawRect.y + 4, 
        drawRect.width, drawRect.height
    }, 0.15f, 16, (Color){0, 0, 0, 100});
    
    DrawRectangleRounded(drawRect, 0.15f, 16, currentColor);
    
    if (button.isHovered) {
        DrawRectangleRoundedLines(drawRect, 0.15f, 16, (Color){255, 255, 255, 100});
    }
    
    Vector2 textSize = MeasureTextEx(font, button.text, 32, 1);
    Vector2 textPos = (Vector2){
        drawRect.x + (drawRect.width - textSize.x) / 2,
        drawRect.y + (drawRect.height - textSize.y) / 2
    };
    
    DrawTextEx(font, button.text, 
        (Vector2){textPos.x + 2, textPos.y + 2}, 
        32, 1, (Color){0, 0, 0, 150});
    
    Color textColor = button.textColor;
    if (button.isHovered) {
        textColor.a = 255;
    }
    DrawTextEx(font, button.text, textPos, 32, 1, textColor);
}
