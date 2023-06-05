#include "../entities/attackAndDefense.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moves.h"
#include "../utils/fileReader.h"

map_t copyMap(map_t * map){
    map_t newMap;
    newMap.name = map->name;
    newMap.monsterCount = map->monsterCount;
    newMap.monsterClassCount = map->monsterClassCount;
    newMap.monsterClass = map->monsterClass;
    newMap.east = map->east;
    newMap.west = map->west;
    newMap.north = map->north;
    newMap.south = map->south;
    newMap.generatedNumber = map->generatedNumber;
    return newMap;
}

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
        printf("Vous avez fui\n");
    }
}

void changeRoom(map_t * map, player_t * player, tabMonsters_t ** tabMonsters, position_t pos, tabMaps_t * tabMaps){
    int isAlreadyGenerated = 0;
    int mapNumber = 0;
    char * mapToGenerate = (char *)malloc(sizeof(char)*50);

    if(pos.y == MAX_SIZE-1 || pos.y == 0) 
        pos.y > 0 ? strcpy(mapToGenerate, map->east) : strcpy(mapToGenerate, map->west);
    else 
        pos.x > 0 ? strcpy(mapToGenerate, map->south) : strcpy(mapToGenerate, map->north);
    
    int cpt=0;
    printf("%d\n", tabMaps->nbMaps);
    printf("%d\n", map->generatedNumber);
    

    while(cpt<tabMaps->nbMaps){
        if (strcmp(tabMaps->maps[cpt].name, mapToGenerate) == 0){
            isAlreadyGenerated = 1;
            break;
        }
        cpt++;
    }

    printf("after while\n");
    
    if(isAlreadyGenerated){
        *map = tabMaps->maps[cpt];
    }
    else {
        *map = initMap(mapToGenerate, tabMonsters);
        tabMaps->maps[tabMaps->nbMaps] = *map;
        tabMaps->nbMaps++;
    }
    
    if(pos.y == MAX_SIZE-1){
        player->pos.y=1;
    }
    else if(pos.y == 0){
        player->pos.y=MAX_SIZE-2;
    }
    else if(pos.x == MAX_SIZE-1){
        player->pos.x=1;
    }
    else{
        player->pos.x=MAX_SIZE-2;
    }
}

void movements(map_t * map, player_t * player, tabMonsters_t ** tabMonsters, int addX, int addY, tabMaps_t * tabMaps){
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
        case '$':
            moveOnPotion(player, newPos.x, newPos.y);
            break;
        case '?':
            changeRoom(map, player, tabMonsters, newPos, tabMaps);
            break;
        case 'A':
            fightResult = startCombat(player, tabMonsters, newPos, map->generatedNumber);
            actionAfterFight(player, newPos.x, newPos.y, fightResult);
            break;
        case 'B':
            fightResult = startCombat(player, tabMonsters, newPos, map->generatedNumber);
            actionAfterFight(player, newPos.x, newPos.y, fightResult);
            break;
        case 'C':
            fightResult = startCombat(player, tabMonsters, newPos, map->generatedNumber);
            actionAfterFight(player, newPos.x, newPos.y, fightResult);
            break;
        case '>' || '<' || '^' || 'v':
            break;
        default:
            break;
    }

    return;
}
