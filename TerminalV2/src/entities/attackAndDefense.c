#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entities.h"
#include "attackAndDefense.h"
#include <math.h>


int combat(player_t * player, monster_t * monster) {

    double damageMonster = player->attack - monster->defense;
    double damagePlayer = monster->attack - player->defense;

    if (damageMonster < 1 ) {
        damageMonster = 1;
    }
    if (damagePlayer  < 1 ) {
        damagePlayer = 1;
    }
    while( player->hp > 0 && monster->hp > 0) {
        monster->hp = monster->hp - damageMonster;
        if(monster->hp <= 0) {
            return 1;
        }
        player->hp = player->hp - damagePlayer;
        if(player->hp <= 0) {
            return 0;
        }
    }


}
