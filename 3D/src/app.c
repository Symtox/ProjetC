/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   Example originally created with raylib 1.3, last time updated with raylib 1.3
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2015-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "./utils.h"
#include "./physics.h"
#include "./Const.h"
#include <stdlib.h>
#include "logger.h"

int drawMap(int ***);
int crouch();
playerPhysics_t physics = INIT_PLAYER_PHYSICS;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    int ** map = NULL;
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    fillWithRandAndWalls(&map);
    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

        /*
        UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.5f -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,    
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
                0.0f                                                // Move up-down
            },
                         // Move to target (zoom)
        */
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);
        handlePlayerMovement(&camera, &physics, map);

        BeginMode3D(camera);
        drawMap(&map);

        EndMode3D();

        // Draw info boxes

        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}





int drawMap(int *** map) {
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            if((*map)[i][j] == 1) {
                DrawCube((Vector3){ i + 0.5, 1.5f, j + 0.5}, 1.0f, 3.0f, 1.0f, WALL_COLOR);
                DrawCubeWires((Vector3){ i + 0.5, 1.5f, j + 0.5}, 1.0f, 3.0f, 1.0f, MAROON);
            } else if((*map)[i][j] == 0) {
                DrawCube((Vector3){ i + 0.5, -0.5f, j+0.5}, 1.0f, 1.0f, 1.0f , CLITERAL(Color){ 0, 158, 47, 255 }); // Draw ground
                DrawCubeWires((Vector3){ i + 0.5, -0.5f, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);

            } else {
                DrawCube((Vector3){ i+ 0.5, -1.5f, j + 0.5}, 1.0f, 1.0f, 1.0f , WHITE); // Draw ground
                DrawCubeWires((Vector3){ i + 0.5, -1.5f, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);

            }
        }
    }
    return 0;
}
