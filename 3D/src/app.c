#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "utils/utils.h"
#include "core/physics.h"
#include <stdlib.h>
#include "board.h"
#include <stdio.h>
#include "core/renderer.h"

#define NUM_FRAMES  3

playerPhysics_t physics = INIT_PLAYER_PHYSICS;


int main(void)
{
    int ** map = NULL;
    const int screenWidth = 800;
    const int screenHeight = 450;

    //Texture2D  button = LoadTexture("assets/button.png");
    //float frameHeight = (float)button.height/NUM_FRAMES;
    //Rectangle sourceRec = { 0, 0, (float)button.width, frameHeight };
    //Rectangle btnBounds = { screenWidth/2.0f - button.width/2.0f, screenHeight/2.0f - button.height/NUM_FRAMES/2.0f, (float)button.width, frameHeight };

    /*int btnState = 0;
    bool btnAction = false;
    Vector2 mousePoint = { 0.0f, 0.0f };*/

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    //DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    map = readCSV("./assets/board.csv", 1, 1);

    bool isInMenu = true; //TODO: faire un enum pour les differentes vues du jeu
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

        /*mousePoint = GetMousePosition();
        btnAction = false;

        if(CheckCollisionPointRec(mousePoint, btnBounds)) {
            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                btnState = 2;
            } else {
                btnState = 1;
            }
            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) btnAction = true;
        } else {
            btnState = 0;
        }
        if(btnAction) {
            isInMenu = false;
        }
        //sourceRec.y = btnState*frameHeight; */

        if(isInMenu) {
            if(IsKeyPressed(KEY_ENTER)) isInMenu = false;
        }
        else {
            if(IsKeyPressed(KEY_ESCAPE)) isInMenu = true;
        }
        BeginDrawing();

        if(isInMenu) {
            ClearBackground(RAYWHITE);
            //DrawTextureRec(button, sourceRec, (Vector2){ btnBounds.x, btnBounds.y }, YELLOW);
        }
        else {
            Render(map, camera);
            handlePlayerMovement(&camera, &physics, map);
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







