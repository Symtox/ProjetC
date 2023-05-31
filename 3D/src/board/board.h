#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H
#include "../utils/const.h"
#include "../../includes/raylib.h"
#include "tiles.h"
typedef struct {
    int x;
    int y;
    int ** chunk;
} chunk_t;

typedef struct {
    int centerX;
    int centerY;
    chunk_t ** chunks;
    int width;
    int height;
} chunkedMap_t;

typedef struct completeMap {
    chunk_t ** chunks;
    int width;
    int height;
} completeMap_t;

void freeMap(chunkedMap_t * map);
void freeChunk(chunk_t * chunk);
int hasLeftChunk(chunkedMap_t, Vector3);
int getHeightFromTileType(tileTypes_e type);
#endif