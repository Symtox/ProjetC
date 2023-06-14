#include "../includes/raylib.h"
#include "../includes/rcamera.h"
#include "utils/utils.h"
#include "core/physics.h"
#include "board/board.h"
#include "core/renderer.h"
#include "core/gameController.h"
#include "menu/mainMenu.h"

int main(void)
{
    chunkedMap_t map;
    player_t player = {0};

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Maze slayer");
    //ToggleFullscreen();
    initLogger();


    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 8.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    player.camera = &camera;

    initMenu();

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

    while (!getQuitGameAction())        // Detect window close button or ESC key
    {
 		BeginDrawing();

        if(!getPlayGameAction()) {
            ClearBackground(RAYWHITE);
			renderMenu();
			handleMenu();
        }
        else {
            benchmarkStart(0, "initGameController");
            initGameController(&player, &map, getSaveName());
            benchmarkEnd(0);
            benchmarkStart(1, "initRenderer");
            initRenderer(&player);
            benchmarkEnd(1);
            benchmarkStart(2, "render");
            Render(map);
            benchmarkEnd(2);
            benchmarkStart(3, "handleGame");
            Tick();
            benchmarkEnd(3);
        }
        benchmarkStart(4, "drawFPS");
        EndDrawing();
        benchmarkEnd(4);

    }
    saveAndQuit();
    endLogger();
    destroyMenu();
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
