#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "utils/utils.h"
#include "core/physics.h"
#include "core/renderer.h"
#include <stdlib.h>
#include "entities.h"

player_t player = INIT_PLAYER;

int main(void)
{
    int ** map = NULL;
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "labyrinthe chelou");

    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    fillWithRandAndWalls(&map);

    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);
        handlePlayerMovement(&camera, &player.physics, map);

        DrawMap(map);

        EndMode3D();

        DrawOverlay(camera);



        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context
    return 0;
}

