#ifndef PROJECT_ENTITIES_H
#define PROJECT_ENTITIES_H
#include "../../includes/raylib.h"
#include "../utils/const.h"
#define BASE_PLAYER { \
    {8, 10, 8, 5}, \
    0, \
    0,         \
    {0, 0, 0, 0, FALL_BASE_SPEED, JUMP_BASE_SPEED, 0} \
}

#define BASE_CAMERA { \
    {15.0f, 10.0f, 15.0f}, \
    {0.0f, 0.0f, 0.0f}, \
    {0.0f, 1.0f, 0.0f}, \
    0.0f, \
    CAMERA_PERSPECTIVE \
}

typedef struct {
    int health;
    int maxHealth;
    int armor;
    int damage;
} statistics_t;

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
    int keyCount;
    playerPhysics_t physics;
}player_t;

typedef struct {
    statistics_t statistics;
    Vector3 position;
    int isDead;
    char class;
}monster_t;
#endif

