#include <stdio.h>
#include <stdlib.h>
//#include <ncurses.h>
#include "../entities/entities.h"

//#define getch() wgetch(stdscr)

typedef struct _plateau {
    char table[10][10];
} plateau;

plateau * createPlateau() {
    plateau * game = malloc(sizeof(plateau));
    for(int i=0; i<10;i++){
        if (i == 0){
            for(int j=0; j<10;j++){
                game->table[i][j] = '#';
            }
        }
        else if (i == 9){
            for(int j=0; j<10;j++){
                game->table[i][j] = '#';
            }
        }
        else{
            for(int j=0; j<10;j++){
                if (j == 0){
                    game->table[i][j] = '#';
                }
                else if (j == 9){
                    game->table[i][j] = '#';
                }
                else{
                    game->table[i][j] = ' ';
                }
            }
        }
        game->table[2][2] = '!';
        game->table[3][6] = '3';
        game->table[7][2] = 'o';
    }
    return game;
}

void printPlateau(plateau * game, player_t player){
    //Suppression de l'objet récupéré / ancienne position du joueur
    game->table[player.pos.x][player.pos.y] = ' ';

    for(int i=0; i<10;i++){
        for(int j=0; j<10;j++){
            if (i == player.pos.x && j == player.pos.y)
                printf("@");
            else
                printf("%c", game->table[i][j]);
        }
        printf("\n");
    }
}

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
        case '?':
            break;
        case '>' || '<' || '^' || 'v':
            break;
        default:
            break;
    }

    return player;
}

void play(plateau * game, player_t player){
    while(1){
        char move;
        scanf("%c", &move); //getch();
        if(move == 'd'){
            player = movements(game, player, 0, 1);
        }
        if(move == 'q'){
            player = movements(game, player, 0, -1);
        }
        if(move == 'z'){
            player = movements(game, player, -1, 0);
        }
        if(move == 's'){
            player = movements(game, player, 1, 0);
        }
        printPlateau(game, player);
    }
}

int main() {

    player_t player = {0, 0, 0, 0, 0, {4, 8}};
    plateau * game = createPlateau();
    printPlateau(game, player);
    play(game, player);
    return 0;
}
