#include "renderer.h"
#define DEBUG_INFO_LINE_COUNT 5
drawBundle_t drawBundle = {0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0};

void initRenderer(player_t * player) {
    drawBundle.player = player;
}

void DrawMap(chunkedMap_t map) {

    for(int i = 0; i < map.width; i++) {
        for(int j = 0; j < map.height; j++) {
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
            if(drawBundle.drawCeiling) {
                DrawCube((Vector3){ i + 0.5, WALL_HEIGHT + 1 - 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color){ 255, 255, 255, 255} );
                DrawCubeWires((Vector3){ i + 0.5, WALL_HEIGHT +1 -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON );
            }
        }

    }
}

void DrawOverlay() {
    if(drawBundle.drawOverlay) {
        DrawRectangle(600, 50, 200, 60, CLITERAL(Color){ 50, 255, 50, 100});
        DrawText(TextFormat("- Health: %03.2f / %03.2f", drawBundle.player->statistics.health,  drawBundle.player->statistics.health), 610, 60, 15, BLACK);
        DrawText(TextFormat("- Armor: %03.2f, Attack: %03.2f", drawBundle.player->statistics.armor, drawBundle.player->statistics.damage), 610, 75, 15, BLACK);
        DrawText(TextFormat("- Keys: %d, Power-up %d", drawBundle.player->inventory.keyCount, drawBundle.player->inventory.potionCount), 610, 90, 15, BLACK);
    }
}

void DrawDebug() {

    const char * infos[DEBUG_INFO_LINE_COUNT] = {
            TextFormat("x: %f, y: %f, z: %f", drawBundle.player->camera->position.x, drawBundle.player->camera->position.y, drawBundle.player->camera->position.z),
            TextFormat("cx: %d, cy: %d", ((int)drawBundle.player->camera->position.x) / CHUNK_SIZE, ((int)drawBundle.player->camera->position.z) / CHUNK_SIZE),
            TextFormat("looking at: (%f, %f, %f)", drawBundle.player->camera->target.x, drawBundle.player->camera->target.y, drawBundle.player->camera->target.z),
            TextFormat("move: (%f, %f, %f)", drawBundle.movement.x, drawBundle.movement.y, drawBundle.movement.z),
            TextFormat("rot: (%f, %f, %f)", drawBundle.direction.x, drawBundle.direction.y, drawBundle.direction.z)
    };

    if(drawBundle.drawDebug) {
        //DrawFPS(10, 10);
        for(int i = 0; i < DEBUG_INFO_LINE_COUNT; i++) {
            DrawText(infos[i], 10, 25 + 15 * i, 10, WHITE);
        }
    }
}
void Render(chunkedMap_t map) {
        ClearBackground(BLACK);

        BeginMode3D(*drawBundle.player->camera);
        DrawMap(map);

        EndMode3D();

        DrawOverlay();
        DrawDebug();

}


drawBundle_t getDrawBundle() {
    return drawBundle;
}

void setDrawBundle(drawBundle_t bundle) {
    drawBundle = bundle;
}