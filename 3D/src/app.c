#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "utils/utils.h"
#include "core/physics.h"
#include <stdlib.h>
#include "board.h"
#include <stdio.h>
#include "core/renderer.h"
#include "menu/mainMenu.h"

playerPhysics_t physics = INIT_PLAYER_PHYSICS;

int main(void)
{
    int ** map = NULL;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 3d camera first person");

    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    map = readCSV("./assets/board.csv", 1, 1);

    while (!WindowShouldClose() && !getQuitGameAction())        // Detect window close button or ESC key
    {
 		BeginDrawing();


        if(!getPlayGameAction()) {
            ClearBackground(RAYWHITE);
			renderMainMenu();
			handleMenu();
        }
        else {
            Render(map, camera);
            handlePlayerMovement(&camera, &physics, map);
            DisableCursor();
        }
        //----------------------------------------------------------------------------------
        EndDrawing();

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}






