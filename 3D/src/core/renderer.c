#include "renderer.h"

int drawCeiling = false;

void DrawMap(map_t map) {

    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            if(map.chunks[i][j].x != -1) {
                DrawChunk(map.chunks[i][j]);
            }
        }
    }
}

void DrawChunk(chunk_t chunk) {
    for(int i = chunk.x * CHUNK_SIZE; i < (chunk.x + 1) * CHUNK_SIZE; i++) {
        for(int j = chunk.y * CHUNK_SIZE; j < (chunk.y + 1) * CHUNK_SIZE; j++) {
            switch (chunk.chunk[i % CHUNK_SIZE][j % CHUNK_SIZE]) {
                case 0:
                    DrawCube((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {255, 255, 255, 255});
                    DrawCubeWires((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    break;
                case 1:
                    for (int k = 0; k < WALL_HEIGHT; k++) {
                        DrawCube((Vector3) {i + 0.5, k + 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f,
                                 CLITERAL(Color) {255, 255, 255, 255});
                        DrawCubeWires((Vector3) {i + 0.5, k + 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    }
                    break;

                case 2:
                    DrawCube((Vector3) {i + 0.5, 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {255, 100, 100, 100});
                    DrawCubeWires((Vector3) {i + 0.5, 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    break;
                default:
                    DrawCube((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {0, 255, 100, 255});
                    DrawCubeWires((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    break;

            }
            if(drawCeiling) {
                DrawCube((Vector3){ i + 0.5, WALL_HEIGHT + 1 - 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color){ 255, 255, 255, 255} );
                DrawCubeWires((Vector3){ i + 0.5, WALL_HEIGHT-0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON );
            }
        }

    }
}

void DrawOverlay(Camera camera) {
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
}

void Render(map_t map, Camera camera) {
        ClearBackground(BLACK);

        BeginMode3D(camera);

            DrawMap(map);

        EndMode3D();

        DrawOverlay(camera);

}

void setDrawCeiling(bool value) {
    drawCeiling = value;
}