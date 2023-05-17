#include "../entities/entities.h"
#include <stdio.h>
#include <stdlib.h>
#include "moves.h"

player_t moveOnKey(player_t player, int x, int y){
    player.nbKey += 1;
    player.pos.x = x;
    player.pos.y = y;
    return player;
}

player_t moveOnDoor(player_t player, int x, int y){
    if (player.nbKey > 0){
        player.nbKey -= 1;
        player.pos.x = x;
        player.pos.y = y;
    }
    return player;
}

player_t moveOnPotion(player_t player, int x, int y){
    player.hp = player.max_hp;
    player.pos.x = x;
    player.pos.y = y;
    return player;
}

player_t moveOnPowerUp(player_t player, int x, int y, int power){
    player.pos.x = x;
    player.pos.y = y;

    if (power == 1) {
        player.attack += 1;
        return player;
    }
    else if(power == 2){
        player.defense += 1;
        return player;
    }
    else{
        player.max_hp += 3;
        return player;
    }
}

player_t movements(plateau * game, player_t player, int addX, int addY){
    position_t newPos = {player.pos.x + addX, player.pos.y + addY};
    
    switch(game->table[newPos.x][newPos.y]){
        case '#':
            break;
        case ' ':
            player.pos.x = newPos.x;
            player.pos.y = newPos.y;
            break;
        case '!':
            player = moveOnKey(player, newPos.x, newPos.y);
            break;
        case 'o':
            player = moveOnDoor(player, newPos.x, newPos.y);
            break;
        case '1':
            player = moveOnPowerUp(player, newPos.x, newPos.y, 1);
            break;
        case '2':
            player = moveOnPowerUp(player, newPos.x, newPos.y, 2);
            break;
        case '3':
            player = moveOnPowerUp(player, newPos.x, newPos.y, 3);
            break;
        case 'P':
            player = moveOnPotion(player, newPos.x, newPos.y);
            break;
        case '?':
            break;
        case '>' || '<' || '^' || 'v':
            break;
        case 'A':

        default:
            break;
    }

    return player;
}
