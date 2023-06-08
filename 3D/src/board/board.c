#include <stdlib.h>
#include "board.h"
#include "../core/renderer.h"

/**
 * Verifie que le joueur est toujours dans le chunk actuel
 */
int hasLeftChunk(chunkedMap_t map, Vector3 playerPos) {
    return (int)playerPos.x / CHUNK_SIZE != map.centerX || (int)playerPos.z / CHUNK_SIZE != map.centerY;
}

/**
 * Free la map
 * @param map
 */
void freeMap(chunkedMap_t * map) {
    for(int i = 0; i < map->width; i++) {
        for(int j = 0; j < map->height; j++) {
            if(map->chunks[i][j].x != -1) {
                freeChunk(&map->chunks[i][j]);
            }
        }
        free(map->chunks[i]);
    }
    free(map->chunks);

}

/**
 * Free un chunk
 * @param chunk
 */
void freeChunk(chunk_t * chunk) {
    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < MAX_Y; y++) {
            free(chunk->chunk[x][y]);
        }
        free(chunk->chunk[x]);
    }
    free(chunk->chunk);
    if(chunk->doorCount > 0) {
        free(chunk->doors);
    }
    if(chunk->powerUpCount > 0) {
        free(chunk->powerUps);
    }
    if(chunk->monsterCount > 0) {
        free(chunk->monsters);
    }
    if(chunk->keyCount > 0) {
        free(chunk->keys);
    }
}

