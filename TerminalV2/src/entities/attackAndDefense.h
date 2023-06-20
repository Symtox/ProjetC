#include "entities.h"

#ifndef MONSTER_DIE

    #define MONSTER_DIE 0
    #define PLAYER_DIE 1
    #define ESCAPE 2

    /*Retourne le monstre à la position pos*/
    monster_t * getMonster(tabMonsters_t ** tabMonsters, position_t pos, int generatedNumber);
    
    /*Affichage du début du combat*/
    void printStartCombat(monster_t * monster, player_t * player);

    /*Affiche les dialogues lors de l'attaque du joueur*/
    void printPlayerAttack(monster_t * monster, player_t * player);

    /*Affiche les dialogues lors de l'attaque du monstre*/
    void printMonsterAttack(monster_t * monster, player_t * player);

    /*Calcul des hp du monstre lors de l'attaque du joueur*/
    void playerAttack(player_t * player, monster_t * monster);

    /*Calcul des hp du joueur lors de l'attaque du monstre*/
    void monsterAttack(monster_t * monster, player_t * player);

    /*Gestion du combat (attaque, fuite,...)*/
    int startCombat(player_t * player, tabMonsters_t ** tabMonsters, position_t pos, int generatedNumber);
    
#endif