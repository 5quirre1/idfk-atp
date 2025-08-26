#include "raylib.h"
#include "game.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "idfk atp"); // init the window
    InitAudioDevice(); // for footstep sounds
    SetTargetFPS(60); // fps

    // icon
    Image icon = LoadImage("assets/dog.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    // run the game loop
    RunGame();

    // cleanup
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
