#include "../../includes/raylib.h"
#include "../utils/const.h"


void DrawOverlay(Camera camera) {
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
}

void DrawMap(int ** map) {
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            DrawCube((Vector3){ i+ 0.5, (float)map[i][j] / 2, j + 0.5}, 1.0f, (float)map[i][j], 1.0f , CLITERAL(Color){ 100 * map[i][j], 100 * map[i][j], 200, 255 }); // Draw ground
            DrawCubeWires((Vector3){ i + 0.5, (float)map[i][j] / 2, j + 0.5}, 1.0f, (float)map[i][j], 1.0f, MAROON);
        }
    }
}
