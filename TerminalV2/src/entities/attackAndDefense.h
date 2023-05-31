#include "entities.h"

#define MONSTER_DIE 0
#define PLAYER_DIE 1
#define ESCAPE 2

monster_t getMonsterByName(char name);

void printStartCombat(monster_t * monster, player_t * player);

void printPlayerAttack(monster_t * monster, player_t * player);

void printMonsterAttack(monster_t * monster, player_t * player);

void playerAttack(player_t * player, monster_t * monster);

void monsterAttack(monster_t * monster, player_t * player);

int startCombat(player_t * player, char monsterName);
