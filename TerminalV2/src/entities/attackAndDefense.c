#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "attackAndDefense.h"
#include "../utils/sleep.h"

monster_t monsters[] = {
    [0] = { .name = 'A', .max_hp = 5, .hp = 5, .defense = 1, .attack = 1 },
    [1] = { .name = 'B', .max_hp = 5, .hp = 5, .defense = 2, .attack = 2 },
    [2] = { .name = 'C', .max_hp = 10, .hp = 10, .defense = 3, .attack = 3 },
    [3] = { .name = 'D', .max_hp = 10, .hp = 10, .defense = 4, .attack = 4 }
}; 

monster_t getMonsterByName(char name){
    for(int i = 0; i < 4; i++){
        if(monsters[i].name == name){
            return monsters[i];
        }
    }
}

void printStats(player_t * player, monster_t * monster){
    printf("Statistiques du joueur  => HP : %d/%d, ATK : %d, DEF : %d\n", player->hp, player->max_hp, player->attack, player->defense);
    printf("Statistiques du monstre => HP : %d/%d, ATK : %d, DEF : %d\n\n", monster->hp, monster->max_hp, monster->attack, monster->defense);
}

void printStartCombat(monster_t * monster, player_t * player){
    system("CLS");
    printf("Vous entrez en combat contre un monstre !\n");
    printStats(player, monster);

    char pass;
    printf("Appuyez sur une touche pour commencer le combat ...");
    scanf("%c", &pass);
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

int startCombat(player_t * player, char monsterName){
    monster_t monster = getMonsterByName(monsterName);
    printStartCombat(&monster, player);

    char choice = ' ';
    while(toupper(choice) != 'F') {
        printPlayerAttack(&monster, player);
        playerAttack(player, &monster);
        if(monster.hp <= 0){
            printf("\nVous avez vaincu le monstre !\n");
            printf("Vous gagnez 1 HP max et 1 ATK\n");
            player->max_hp++;
            player->attack++;
            wait(2000);
            system("CLS");
            return MONSTER_DIE;
        }
        printMonsterAttack(&monster, player);
        monsterAttack(&monster, player);
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
            printStats(player, &monster);
            printf("Que voulez-vous faire ?\n    F : Fuir\n    A : Attaquer\nVotre choix : ");
            scanf("%c", &choice);
        }
    }

    printf("\nVous avez fui le combat !\n");
    wait(2000);

    return ESCAPE;
}
