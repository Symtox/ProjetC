#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H
#include "../utils/const.h"
#include "../../includes/raylib.h"
#include "tiles.h"
#include "entities.h"

typedef struct {
    int opened;
    int rotation;
    Vector3 position;
} door_t;

typedef struct {
    int pickedUp;
    Vector3 position;
} potion_t;

typedef struct {
    int pickedUp;
    int type;
    Vector3 position;
} powerUp_t;

typedef struct {
    int pickedUp;
    Vector3 position;
} DoorKey_t;

typedef struct {
    int x;
    int y;
    int *** chunk;

    door_t * doors;
    int doorCount;

    powerUp_t * powerUps;
    int powerUpCount;

    potion_t * potions;
    int potionCount;

    monster_t * monsters;
    int monsterCount;

    DoorKey_t * keys;
    int keyCount;
} chunk_t;


typedef struct {
    int x;
    int y;
    int *** chunk;

    door_t * doors;
    int doorCount;

    powerUp_t * powerUps;
    int powerUpCount;

    potion_t * potions;
    int potionCount;

    monster_t * monsters;
    int monsterCount;

    DoorKey_t * keys;
    int keyCount;

    char * east;
    char * north;
    char * west;
    char * south;
} chunk_txt;

typedef struct {
    int centerX;
    int centerY;
    chunk_t ** chunks;
    int width;
    int height;
} chunkedMap_t;


void freeMap(chunkedMap_t * map);
void freeChunk(chunk_t * chunk);
int hasLeftChunk(chunkedMap_t, Vector3);
int getHeightFromTileType(tileTypes_e type);
#endif