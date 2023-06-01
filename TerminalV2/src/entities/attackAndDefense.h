#include "entities.h"

#ifndef MONSTER_DIE

    #define MONSTER_DIE 0
    #define PLAYER_DIE 1
    #define ESCAPE 2

    monster_t * getMonster(tabMonsters_t ** tabMonsters, position_t pos, int generatedNumber);
    
    void printStartCombat(monster_t * monster, player_t * player);

    void printPlayerAttack(monster_t * monster, player_t * player);

    void printMonsterAttack(monster_t * monster, player_t * player);

    void playerAttack(player_t * player, monster_t * monster);

    void monsterAttack(monster_t * monster, player_t * player);

    int startCombat(player_t * player, tabMonsters_t ** tabMonsters, position_t pos, int generatedNumber);
    
#endif