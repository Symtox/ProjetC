#include "core/physics.h"
#define INIT_PLAYER_STATS (statistics_t) {100, 100, 10, 10}
#define INIT_PLAYER (player_t) { \
    {0, 0, 0, 0, FALL_BASE_SPEED, JUMP_BASE_SPEED}, \
    {100, 100, 10, 10},         \
    0                           \
}

typedef struct statistics_t {
    int max_hp;
    int hp;
    int defense;
    int attack;
} statistics_t;

typedef struct player_t {
    playerPhysics_t physics;
    statistics_t stats;
    int nbKey;
} player_t;

typedef struct monster_t {
    statistics_t stats;
} monster_t;


