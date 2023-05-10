#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "../entities/entities.h"

typedef struct _plateau {
    char table[10][10];
} plateau;

plateau * createPlateau() {
    plateau * game = (plateau *)(malloc(sizeof(plateau *)));
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
    }
    return game;
}

void printPlateau(plateau * game, player_t player){
    for(int i=0; i<10;i++){
        for(int j=0; j<10;j++){
            if(player.posX == i && player.posY == j)
                printf("@");
            else
                printf("%c", game->table[i][j]);
        }
        printf("\n");
    }
}

void play(plateau * game, player_t player){
    while(1){
        char move;
        move = getch();
        if(move == 'd'){
            if(game->table[player.posX][player.posY+1] != '#');
                player.posY++;
        }
        if(move == 'q'){
            if(game->table[player.posX][player.posY-1] != '#');
                player.posY--;
        }
        if(move == 'z'){
            if(game->table[player.posX-1][player.posY] != '#');
                player.posX--;
        }
        if(move == 's'){
            if(game->table[player.posX+1][player.posY] != '#');
                player.posX++;
        }
        printPlateau(game, player);
    }
}

int main() {
    player_t player = {0, 0, 0, 0, 0, 4, 8};
    plateau * game = createPlateau();
    printPlateau(game, player);
    play(game, player);
    return 0;
}