#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <windows.h>
#include "entities/entities.h"
#include "movements/moves.h"
#include "main.h"
#include "utils/fileReader.h"
#include <locale.h>

void credits(){
    system("CLS");
    printf("Credits de Maze Slayer CLI\n\n");
    printf("Developpeurs :\n");
    printf("\t- Clement MABILE\n");
    printf("\t- Enzo DUPRE\n");
    printf("\t- Maxime MILLAT\n");
    printf("\t- Theo MOUISSE\n\n");
    printf("Derniere date de modification : 05/06/2023\n\n");
    printf("Appuyez sur une touche pour revenir au menu principal...");
    _getch();
    return;
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
        case '$':
            color(12, 0);
            printf("%lc", L'§');
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

void printMap(map_t * map, player_t * player){
    //Suppression de l'objet récupéré / ancienne position du joueur
    map->table[player->pos.x][player->pos.y] = ' ';

    for(int i=0; i<MAX_SIZE;i++){
        for(int j=0; j<MAX_SIZE;j++){
            if (i == player->pos.x && j == player->pos.y)
                printf("@");
            else
                printItemColor(map->table[i][j]);
            
            /*Affichage des stats du joueur*/
            if (i == 1 && j == MAX_SIZE-1) {
                color(12,0);
                printf(" HP ");
                color(15,0);
                printf(" : %d/%d", player->hp, player->max_hp);
            }
            else if (i == 2 && j == MAX_SIZE-1) {
                color(10, 0);
                printf(" ATK ");
                color(15,0);
                printf(": %d", player->attack);
            }
            else if (i == 3 && j == MAX_SIZE-1) {
                color(9, 0);
                printf(" DEF ");
                color(15,0);
                printf(": %d", player->defense);
            }
            else if (i == 4 && j == MAX_SIZE-1) {
                color(14, 0);
                printf(" KEY ");
                color(15,0);
                printf(": %d", player->nbKey);
            }

            /*Affichage des classes de monstre*/
            if(map->monsterClassCount > 0)
            {
                if (i == 8 && j == MAX_SIZE-1) {
                    printf("\tClasse : %c", map->monsterClass[0].name);
                }
                else if (i == 9 && j == MAX_SIZE-1) {
                    printf("\tHP : %d", map->monsterClass[0].max_hp);
                }
                else if (i == 10 && j == MAX_SIZE-1) {
                    printf("\tATK : %d", map->monsterClass[0].attack);
                }
                else if (i == 11 && j == MAX_SIZE-1) {
                    printf("\tDEF : %d", map->monsterClass[1].defense);
                }
            }

            if(map->monsterClassCount > 1)
            {
                if (i == 13 && j == MAX_SIZE-1) {
                    printf("\tClasse : %c", map->monsterClass[1].name);
                }
                else if (i == 14 && j == MAX_SIZE-1) {
                    printf("\tHP : %d", map->monsterClass[1].max_hp);
                }
                else if (i == 15 && j == MAX_SIZE-1) {
                    printf("\tATK : %d", map->monsterClass[1].attack);
                }
                else if (i == 16 && j == MAX_SIZE-1) {
                    printf("\tDEF : %d", map->monsterClass[1].defense);
                }
            }

            if(map->monsterClassCount > 2)
            {
                if (i == 18 && j == MAX_SIZE-1) {
                    printf("\tClasse : %c", map->monsterClass[2].name);
                }
                else if (i == 19 && j == MAX_SIZE-1) {
                    printf("\tHP : %d", map->monsterClass[2].max_hp);
                }
                else if (i == 20 && j == MAX_SIZE-1) {
                    printf("\tATK : %d", map->monsterClass[2].attack);
                }
                else if (i == 21 && j == MAX_SIZE-1) {
                    printf("\tDEF : %d", map->monsterClass[2].defense);
                }
            }
        }
        printf("\n");
    }
}

void play(map_t * map, player_t * player, tabMonsters_t ** tabMonsters, tabMaps_t * tabMaps){
    char move;
    // Boucle de jeu
    while(1){
        move = _getch();
        // ->
        if(move == 77){
            movements(map, player, tabMonsters, 0, 1, tabMaps);
        }
        // <-
        else if(move == 75){
            movements(map, player, tabMonsters, 0, -1, tabMaps);
        }
        // ^
        else if(move == 72){
            movements(map, player, tabMonsters, -1, 0, tabMaps);
        }
        // v
        else if(move == 80){
            movements(map, player, tabMonsters, 1, 0, tabMaps);
        }
        else if(move == 'q'){
            break;
        }
        else if(move == '*'){
            player->max_hp = 999;
            player->hp = 999;
            player->attack = 999;
            player->defense = 999;
        }
        else if(move == '!'){
            player->nbKey = 999;
        }
        else {
            continue;
        }
        system("CLS");
        printMap(map, player);
    }
    return;
}

void endGame(tabMaps_t * tabMaps, tabMonsters_t ** tabMonsters){
    for(int i=0;i<tabMaps->nbMaps;i++){
        free(tabMonsters[i]->monsters);
    }

    for(int i=0;i<tabMaps->nbMaps;i++){
        free(tabMaps->maps[i].monsterClass);
        free(tabMaps->maps[i].table);
    }

    free(tabMaps->maps);
    free(tabMaps);

    return;
}

void startNewGame(){
    /*Initialisation du jeu (joueur, map, monstres)*/
    player_t player = {10, 10, 1, 2, 0, {14, 14}};
    tabMonsters_t ** tabMonsters = (tabMonsters_t **)malloc(sizeof(tabMonsters_t *) * MAX_ROOMS);
    tabMaps_t * tabMaps = (tabMaps_t *)malloc(sizeof(tabMaps_t)*MAX_ROOMS);
    tabMaps->maps = (map_t *)malloc(sizeof(map_t)*MAX_ROOMS);
    map_t map = initMap("niveau1.level", tabMonsters, 1);
    tabMaps->maps[0] = map;
    tabMaps->nbMaps = 1;

    /*Début du jeu*/
    system("CLS");
    printMap(&map, &player);
    play(&map, &player, tabMonsters, tabMaps);

    endGame(tabMaps, tabMonsters);

    return;
}

void startMenu(){
    setlocale(LC_ALL, "");
    char choice;
    while(toupper(choice) != 'N' && toupper(choice) != 'C' && toupper(choice) != 'Q'){
        choice = ' ';
        system("CLS");
        printf("Bienvenue dans Maze Slayer CLI !\n\n");
        printf("[N]ouvelle partie\n");
        printf("[C]redits\n");
        printf("[Q]uitter\n");
        choice = _getch();
    
        switch(toupper(choice)){
            case 'N':
                startNewGame();
                break;
            case 'C':
                credits();
                break;
            case 'Q':
                exit(0);
                break;
        }
        choice = ' ';
    }

    return;
}

int main() {
    startMenu();
    return 0;
}
