#ifndef PROJECT_ENTITIES_H
#define PROJECT_ENTITIES_H
#include "../includes/raylib.h"
#include "utils/const.h"
#define BASE_PLAYER { \
    {10, 10, 1, 2, 0, 0}, \
    0, \
    {0, 0},         \
    {0, 0, 0, 0, FALL_BASE_SPEED, JUMP_BASE_SPEED, 0} \
}

typedef struct {
    float health;
    float maxHealth;
    float armor;
    float damage;
    int level;
    int experience;
} statistics_t;

typedef struct {
    int keyCount;
    int potionCount;
} inventory_t;

typedef struct {
    int isJumping;
    int isFalling;
    int jumpTime;
    int fallTime;
    float fallingSpeed;
    float jumpingSpeed;
    int noclip;
}playerPhysics_t;

typedef struct {
    statistics_t statistics;
    Camera * camera;
    inventory_t inventory;
    playerPhysics_t physics;
}player_t;
#endif

