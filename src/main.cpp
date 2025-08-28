#include "raylib.h"
#include "game.h"
#include "menu.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "idfk"); // init the window
    InitAudioDevice(); // for footstep sounds and menu sounds
    SetTargetFPS(60); // fps
    SetExitKey(0); // don't make esc close window

    // icon
    Image icon = LoadImage("assets/dog.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    // initialize menu
    Menu menu = InitMenu();
    MenuState gameState = MENU_STATE_MAIN;
    bool inGame = false;

    // main loop
    while (!WindowShouldClose() && gameState != MENU_STATE_EXIT) {
        if (!inGame) {
            gameState = UpdateMenu(&menu);
            
            if (gameState == MENU_STATE_GAME) {
                inGame = true;
                EnableCursor();
                DisableCursor();
            }
            
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawMenu(&menu);
            EndDrawing();
        } else {
            if (IsKeyPressed(KEY_ESCAPE)) {
                inGame = false;
                gameState = MENU_STATE_MAIN;
                EnableCursor();
                continue;
            }
            RunGame();
            inGame = false;
            gameState = MENU_STATE_MAIN;
            EnableCursor();
        }
    }

    UnloadMenu(&menu);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
