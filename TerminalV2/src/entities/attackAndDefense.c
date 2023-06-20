#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "attackAndDefense.h"
#include <math.h>
#include <ctype.h>
#include "../utils/sleep.h"
#include <conio.h>


monster_t * getMonster(tabMonsters_t ** tabMonsters, position_t pos, int generatedNumber){
    for(int i=0;i<tabMonsters[generatedNumber]->nbMonsters;i++){
        if (tabMonsters[generatedNumber]->monsters[i].pos.x == pos.x &&
            tabMonsters[generatedNumber]->monsters[i].pos.y == pos.y)
        {
            return &tabMonsters[generatedNumber]->monsters[i];
        }
    }
    return NULL;
}

void printStats(player_t * player, monster_t * monster){
    printf("Statistiques du joueur  => HP : %d/%d, ATK : %d, DEF : %d\n", player->hp, player->max_hp, player->attack, player->defense);
    printf("Statistiques du monstre => HP : %d/%d, ATK : %d, DEF : %d\n\n", monster->hp, monster->max_hp, monster->attack, monster->defense);
}

void printStartCombat(monster_t * monster, player_t * player){
    system("CLS");
    printf("Vous entrez en combat contre un monstre !\n");
    printStats(player, monster);

    printf("Appuyez sur une touche pour commencer le combat...");
    _getch();
}

void printPlayerAttack(monster_t * monster, player_t * player){
    system("CLS");
    printStats(player, monster);

    printf("Vous infligez %d de degat(s) au monstre\n", player->attack>monster->defense ? player->attack - monster->defense : 1);
    wait(800);
    int hp_lost = monster->hp - (player->attack>monster->defense ? player->attack - monster->defense : 1);
    printf("Le monstre a %d HP\n", hp_lost > 0 ? hp_lost : 0);
    wait(800);
}

void printMonsterAttack(monster_t * monster, player_t * player){
    printf("Le monstre vous inflige %d degat(s)\n", monster->attack>player->defense ? monster->attack - player->defense : 1);
    wait(800);
    printf("Vous perdez %d HP\n\n", monster->attack>player->defense ? monster->attack - player->defense : 1);
    wait(800);
}

void playerAttack(player_t * player, monster_t * monster) {
    monster->hp = monster->hp - (player->attack>monster->defense ? player->attack - monster->defense : 1);
}

void monsterAttack(monster_t * monster, player_t * player) {
    player->hp = player->hp - (monster->attack>player->defense ? monster->attack - player->defense : 1);
}

int startCombat(player_t * player, tabMonsters_t ** tabMonsters, position_t pos, int generatedNumber){
    monster_t * monster = getMonster(tabMonsters, pos, generatedNumber);
    printStartCombat(monster, player);

    char choice = ' ';
    while(toupper(choice) != 'F') {
        printPlayerAttack(monster, player);
        playerAttack(player, monster);
        if(monster->hp <= 0){
            printf("\nVous avez vaincu le monstre !\n");
            printf("Vous gagnez 1 HP max et 1 ATK\n");
            player->max_hp++;
            player->hp++;
            player->attack++;
            wait(2000);
            system("CLS");
            return MONSTER_DIE;
        }
        printMonsterAttack(monster, player);
        monsterAttack(monster, player);
        if(player->hp <= 0){
            printf("\nVous avez perdu !\n");
            wait(2000);
            system("CLS");
            return PLAYER_DIE;
        }

        wait(800);

        choice = ' ';
        while(toupper(choice) != 'F' && toupper(choice) != 'A'){
            choice = ' ';
            system("CLS");
            printStats(player, monster);
            printf("Que voulez-vous faire ?\n    F : Fuir\n    A : Attaquer\nVotre choix : ");
            choice = _getch();
        }
    }

    printf("\nVous avez fui le combat !\n");
    wait(2000);

    return ESCAPE;
}
