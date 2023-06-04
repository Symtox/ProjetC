#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H
#include "../utils/const.h"
#include "../../includes/raylib.h"
#include "entities.h"
#define POTION_HEALING 1
#define POWER_UP_ATTACK 1
#define POWER_UP_DEFENSE 1
#define POWER_UP_MAX_HP 1
typedef struct {
    int opened;
    Vector3 position;
} door_t;

typedef struct {
    int pickedUp;
    Vector3 position;
} potion_t;

typedef enum {
    ATTACK = 1,
    DEFENSE = 2,
    MAX_HP = 3
}powerUpType_e;

typedef struct {
    int pickedUp;
    powerUpType_e type;
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
#endif