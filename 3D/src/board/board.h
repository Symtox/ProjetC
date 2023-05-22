#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H
#include "../utils/const.h"
#include "../../includes/raylib.h"
typedef struct {
    int x;
    int y;
    int ** chunk;
} chunk_t;

typedef struct {
    int centerX;
    int centerY;
    chunk_t chunks[MAP_SIZE][MAP_SIZE];
} map_t;

char * getCharFromLine(char*, int, int);
void loadChunk(char*, chunk_t *, int, int);
map_t loadMap(char *, int, int);
void freeMap(map_t * map);
void freeChunk(chunk_t * chunk);
int hasLeftChunk(map_t, Vector3);
void loadCurrentMap(map_t *, Vector3);

#endif