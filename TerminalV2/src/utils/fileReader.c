#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>
#include "../entities/entities.h"

int nbRoomGenerated = 0;

int isMonsterClassFound(char * monsters, char monster, int nbClassFound) {
    for(int i = 0; i <= nbClassFound; i++) {
        if(monsters[i] == monster) {
            return 1;
        }
    }
    return 0;
}


map_t readTXT(char * path){
    FILE *file;
    map_t map = {0};
    char caractere;
    int i = 0;
    int j = 0;
    char monstersClassFound[26] = {0};

    map.monsterCount = 0;
    map.monsterClassCount = 0;

    // Ouvrir le fichier en mode lecture
    file = fopen(path, "r");

    // Vérifier si le fichier est ouvert avec succès
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
    }

    for(i=0; i < MAX_SIZE; i++) {
        for (j = 0; (caractere = fgetc(file)) != EOF && j < MAX_SIZE; j++) {
            if(caractere >= 'A' && caractere <= 'Z' ) {
                if(!isMonsterClassFound(monstersClassFound, caractere, map.monsterClassCount)) {
                    monstersClassFound[map.monsterClassCount] = caractere;
                    map.monsterClassCount++;
                }
                map.monsterCount++;
            }
            map.table[i][j] = caractere;
        }
    }

    // Fermer le fichier
    fclose(file);

    return map;
}

char * substr(char *src, int pos) {
    char *dest = malloc(sizeof(char*)*1000);

    int i = pos;
    int cpt = 0;
    for(i; src[i] != '\0'; i++) {
        dest[cpt]=src[i];
        cpt++;
    }

    return dest;
}

map_t initMap(char * fileName, tabMonsters_t ** tabMonsters) {
    FILE *file;
    char* ligne = NULL;
    int longueur = 0;
    int tailleLigne = 100;

    char * path =(char*) malloc((strlen(fileName) + 1 + strlen("../../bin/levels/"))*sizeof(char)); // +1 for the null-terminator
    strcpy(path, "../../bin/levels/");
    strcat(path, fileName);

    map_t map = readTXT(path);
    file = fopen(path, "r");
    map.name = fileName;

    int monsterIndex = 0;
    tabMonsters[nbRoomGenerated]->levelName = fileName;
    nbRoomGenerated++;

    int cptRoom = 0;
    for(int i=0;i<nbRoomGenerated;i++){
        if(tabMonsters[i]->levelName == fileName){
            cptRoom++;
            break;
        }
    }
    tabMonsters[cptRoom]->monsters = (monster_t*)malloc(sizeof(monster_t) * map.monsterCount);
    map.monsterClass = (monsterClass_t*)malloc(sizeof(monsterClass_t) * map.monsterClassCount);

    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
    }

    while ((tailleLigne = getline(&ligne, &longueur, file)) != -1) {

        if (strstr(ligne,"Est")!= NULL){
            char * mapName = substr(ligne, 6);
            map.east = mapName;
        }
        else if (strstr(ligne,"Sud")!= NULL){
            char * mapName = substr(ligne, 6);
            map.south = mapName;
        }
        else if (strstr(ligne, "Ouest") != NULL) {
            char * mapName = substr(ligne, 8);
            map.west = mapName;
        }
        else if (strstr(ligne,"Nord")!= NULL){
            char * mapName = substr(ligne, 7);
            map.north = mapName;
        }
        else if(ligne[0] >= 'A' && ligne[0] <= 'Z') {
            printf("Reading monster class : %d %c\n", monsterIndex, ligne[0]);
            map.monsterClass[monsterIndex].name = (char)ligne[0];
            getline(&ligne, &longueur, file);
            map.monsterClass[monsterIndex].max_hp = atoi(substr(ligne,5));

            getline(&ligne, &longueur, file);
            map.monsterClass[monsterIndex].attack = atoi(substr(ligne,8));

            getline(&ligne, &longueur, file);
            map.monsterClass[monsterIndex].defense = atoi(substr(ligne,8));
            monsterIndex++;
        }
    }

    free(ligne);
    fclose(file);

    monsterIndex = 0;
    for(int i=0;i<MAX_SIZE;i++){
        for(int j=0;j<MAX_SIZE;j++){
            if(map.table[i][j] >= 'A' && map.table[i][j] <= 'Z') {
                for(int cpt;cpt<map.monsterClassCount;cpt++){
                    if(map.monsterClass[cpt].name == map.table[i][j]){
                        tabMonsters[cptRoom]->monsters[monsterIndex].name    = monsterIndex;
                        tabMonsters[cptRoom]->monsters[monsterIndex].max_hp  = map.monsterClass[cpt].max_hp;
                        tabMonsters[cptRoom]->monsters[monsterIndex].hp      = map.monsterClass[cpt].max_hp;
                        tabMonsters[cptRoom]->monsters[monsterIndex].attack  = map.monsterClass[cpt].attack;
                        tabMonsters[cptRoom]->monsters[monsterIndex].defense = map.monsterClass[cpt].defense;
                        tabMonsters[cptRoom]->monsters[monsterIndex].pos.x   = i;
                        tabMonsters[cptRoom]->monsters[monsterIndex].pos.y   = j;
                        monsterIndex++;
                    }
                }
            }
        }
    }
    
    return map;
}