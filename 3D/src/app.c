#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "utils/utils.h"
#include "core/physics.h"
#include "board/board.h"
#include "core/renderer.h"
#include "core/gameController.h"


int main(void)
{
    chunkedMap_t map;
    player_t player = BASE_PLAYER;
    const int screenWidth = 800;
    const int screenHeight = 450;

    int loadMapFromSave = 0;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");
    DisableCursor();                    // Limit cursor to relative movement inside the window

    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    player.camera = &camera;

    initLogger();
    initRenderer(&player);
    initGameController(&player, &map, loadMapFromSave);

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        BeginDrawing();
        Tick(&map);
        Render(map);
        //----------------------------------------------------------------------------------
        EndDrawing();

    }
    endLogger();
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}



