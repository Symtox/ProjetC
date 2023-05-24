#include <stdlib.h>
#include "board.h"
#include "../core/renderer.h"

int hasLeftChunk(chunkedMap_t map, Vector3 playerPos) {
    return (int)playerPos.x / CHUNK_SIZE != map.centerX || (int)playerPos.z / CHUNK_SIZE != map.centerY;
}


void freeMap(chunkedMap_t * map) {

    for(int i = 0; i < map->width; i++) {
        for(int j = 0; j < map->height; j++) {
            if(map->chunks[i][j].x != -1) {
                freeChunk(&map->chunks[i][j]);
            }
        }
    }
}

void freeChunk(chunk_t * chunk) {
    for(int i = 0; i < CHUNK_SIZE; i++) {
        free(chunk->chunk[i]);
    }
    free(chunk->chunk);
}

