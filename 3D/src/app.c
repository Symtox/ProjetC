#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "utils/utils.h"
#include "core/physics.h"
#include <stdlib.h>
#include "board.h"
#include <stdio.h>
#include "core/renderer.h"

playerPhysics_t physics = INIT_PLAYER_PHYSICS;


int main(void)
{
    int ** map = NULL;
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    map = readCSV("./assets/board.csv", 0, 0);
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        BeginDrawing();

        Render(map, camera);
        handlePlayerMovement(&camera, &physics, map);
        //----------------------------------------------------------------------------------
        EndDrawing();

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}







