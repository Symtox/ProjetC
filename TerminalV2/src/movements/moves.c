#include "../entities/attackAndDefense.h"
#include <stdio.h>
#include <stdlib.h>
#include "moves.h"

void moveOnKey(player_t * player, int x, int y){
    player->nbKey += 1;
    player->pos.x = x;
    player->pos.y = y;
    return;
}

void moveOnDoor(player_t * player, int x, int y){
    if (player->nbKey > 0){
        player->nbKey -= 1;
        player->pos.x = x;
        player->pos.y = y;
    }
    return;
}

void moveOnPotion(player_t * player, int x, int y){
    player->hp = player->max_hp;
    player->pos.x = x;
    player->pos.y = y;
    return;
}

void moveOnPowerUp(player_t * player, int x, int y, int power){
    player->pos.x = x;
    player->pos.y = y;

    if (power == 1) {
        player->attack += 1;
    }
    else if(power == 2){
        player->defense += 1;
    }
    else{
        player->max_hp += 3;
    }

    return;
}


void actionAfterFight(player_t * player, int x, int y, int fightResult){
    if (fightResult == MONSTER_DIE){
        player->pos.x = x;
        player->pos.y = y;
    }
    else if (fightResult == PLAYER_DIE){
        printf("+-------------------------+\n");
        printf("|        GAME OVER        |\n");
        printf("+-------------------------+\n");
        exit(0);
    }
    else{
        printf("You escaped\n");
    }
}

void movements(map_t * map, player_t * player, int addX, int addY){
    position_t newPos = {player->pos.x + addX, player->pos.y + addY};
    
    int fightResult = -1;

    switch(map->table[newPos.x][newPos.y]){
        case '#':
            break;
        case ' ':
            player->pos.x = newPos.x;
            player->pos.y = newPos.y;
            break;
        case '!':
            moveOnKey(player, newPos.x, newPos.y);
            break;
        case 'o':
            moveOnDoor(player, newPos.x, newPos.y);
            break;
        case '1':
            moveOnPowerUp(player, newPos.x, newPos.y, 1);
            break;
        case '2':
            moveOnPowerUp(player, newPos.x, newPos.y, 2);
            break;
        case '3':
            moveOnPowerUp(player, newPos.x, newPos.y, 3);
            break;
        case 'P':
            moveOnPotion(player, newPos.x, newPos.y);
            break;
        case '?':
            break;
        case 'A':
            fightResult = startCombat(player, 'A');
            actionAfterFight(player, newPos.x, newPos.y, fightResult);
            break;
        case 'B':
            fightResult = startCombat(player, 'B');
            actionAfterFight(player, newPos.x, newPos.y, fightResult);
            break;
        case 'C':
            fightResult = startCombat(player, 'C');
            actionAfterFight(player, newPos.x, newPos.y, fightResult);
            break;
        case '>' || '<' || '^' || 'v':
            break;
        default:
            break;
    }

    return;
}
