#include "renderer.h"
#include "../utils/const.h"

void DrawMap(int ** map) {
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            if(map[i][j] == 0) {
                DrawCube((Vector3){ i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color){ 255, 255, 255, 255} );
                DrawCubeWires((Vector3){ i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON );
            } else {
                for(int k = 0; k < map[i][j]; k++) {
                    DrawCube((Vector3){ i + 0.5, k + 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color){ 255, 255, 255, 255} );
                    DrawCubeWires((Vector3){ i + 0.5, k + 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON );
                }
            }
        }
    }
}

void DrawOverlay(Camera camera) {
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
}

void Render(int ** map, Camera camera) {
        ClearBackground(BLACK);

        BeginMode3D(camera);

            DrawMap(map);

        EndMode3D();

        DrawOverlay(camera);
        DrawMap(map);

}