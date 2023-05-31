#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "entities/entities.h"
#include "movements/moves.h"
#include "main.h"


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
        game->table[3][3] = '3';
        game->table[7][7] = 'o';
        game->table[1][1] = '2';
        game->table[8][8] = '1';
        game->table[5][5] = 'P';
        game->table[6][6] = 'A';
    }
    return game;
}

void color(int t,int f)
{
        HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(H,f*16+t);
}

void printItemColor(char item){
    switch(item){
        case '!':
            color(14, 0);
            printf("%c", item);
            break;
        case '1':
            color(10, 0);
            printf("%c", item);
            break;
        case '2':
            color(9, 0);
            printf("%c", item);
            break;
        case '3':
            color(5, 0);
            printf("%c", item);
            break;
        case 'P':
            color(12, 0);
            printf("%c", item);
            break;
        case 'o':
            color(8, 0);
            printf("%c", item);
            break;
        default:
            printf("%c", item);
            break;
    }
    color(15,0);
}

void printPlateau(plateau * game, player_t * player){
    //Suppression de l'objet récupéré / ancienne position du joueur
    game->table[player->pos.x][player->pos.y] = ' ';

    for(int i=0; i<10;i++){
        for(int j=0; j<10;j++){
            if (i == player->pos.x && j == player->pos.y)
                printf("@");
            else
                printItemColor(game->table[i][j]);
            
            if (i == 1 && j == 9) {
                color(12,0);
                printf(" HP ");
                color(15,0);
                printf(" : %d/%d", player->hp, player->max_hp);
            }
            else if (i == 2 && j == 9) {
                color(10, 0);
                printf(" ATK ");
                color(15,0);
                printf(": %d", player->attack);
            }
            else if (i == 3 && j == 9) {
                color(9, 0);
                printf(" DEF ");
                color(15,0);
                printf(": %d", player->defense);
            }
            else if (i == 4 && j == 9) {
                color(14, 0);
                printf(" KEY ");
                color(15,0);
                printf(": %d", player->nbKey);
            }
        }
        printf("\n");
    }
}

void play(plateau * game, player_t * player){
    int move = 0;
    while(1){
        char move = _getch();
        if(move == 77){
            movements(game, player, 0, 1);
        }
        else if(move == 75){
            movements(game, player, 0, -1);
        }
        else if(move == 72){
            movements(game, player, -1, 0);
        }
        else if(move == 80){
            movements(game, player, 1, 0);
        }
        else if (move == 'q'){
            break;
        }
        else {
            continue;
        }
        system("CLS");
        printPlateau(game, player);
    }
}

int main() {
    player_t player = {10, 10, 1, 2, 0, {4, 8}};
    plateau * game = createPlateau();
    system("CLS");
    printPlateau(game, &player);
    play(game, &player);
    return 0;
}