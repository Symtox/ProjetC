#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>
#include "../entities/entities.h"

// Variable globale qui compte le nombre de maps différentes générées
int nbRoomGenerated = 0;

int isMonsterClassFound(char * monsters, char monster, int nbClassFound) {
    for(int i = 0; i <= nbClassFound; i++) {
        if(monsters[i] == monster) {
            return 1;
        }
    }
    return 0;
}


map_t readMap(char * path){
    FILE *file;
    map_t map = {0};
    wint_t caractere;
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
        for (j = 0;j < MAX_SIZE; j++) {
            caractere = fgetwc(file);
            if(caractere >= 'A' && caractere <= 'Z' ) {
                if(!isMonsterClassFound(monstersClassFound, caractere, map.monsterClassCount)) {
                    monstersClassFound[map.monsterClassCount] = caractere;
                    map.monsterClassCount++;
                }
                map.monsterCount++;
            }

            // Gestion du caractère spécial §
            if(caractere == 194){
                map.table[i][j] = '$';
                fgetwc(file);
            }
            else{
                map.table[i][j] = caractere;
            }
        }
        fgetwc(file);
    }
    // Fermer le fichier
    fclose(file);

    return map;
}

char * substr(char *src, int pos) {
    if(pos > strlen(src) || pos < 0 || src == NULL || src[pos] == '\0' || src[pos] == '\n' || src[pos] == '\r') {
        return NULL;
    }
    char *dest = malloc(sizeof(char) * (strlen(src) - pos + 1));

    int cpt = 0;
    for(int i = pos; src[i] != '\0' && src[i] != '\n' && src[i] != '\r' ; i++) {
        dest[cpt]=src[i];
        cpt++;
    }
    dest[cpt] = '\0';
    return dest;
}

map_t initMap(char * fileName, tabMonsters_t ** tabMonsters, int isFirstInit) {
    FILE *file;
    if(isFirstInit) nbRoomGenerated = 0;

    char * path =(char*) malloc((strlen(fileName) + 1 + strlen("./bin/levels/"))*sizeof(char)); // +1 pour le caractère de fin de chaine
    strcpy(path, "./bin/levels/");
    strcat(path, fileName);
    
    map_t map = readMap(path);
    map.name = fileName;

    // Allocation de la mémoire pour le tableau de monstres
    tabMonsters[nbRoomGenerated] = (tabMonsters_t *)malloc(sizeof(tabMonsters_t));
    tabMonsters[nbRoomGenerated]->levelName = fileName;
    map.generatedNumber = nbRoomGenerated;

    tabMonsters[nbRoomGenerated]->nbMonsters = map.monsterCount;
    tabMonsters[nbRoomGenerated]->monsters = (monster_t*)malloc(sizeof(monster_t) * map.monsterCount);
    map.monsterClass = (monsterClass_t *)malloc(sizeof(monsterClass_t)*map.monsterClassCount);

    file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
    }

    char * line = NULL;
    size_t len = 0;
    int cptMonsterClass = 0;
    // Lecture du fichier des niveaux adjacents et des classes de monstre
    while (getline(&line, &len, file) != -1) {

        if (strstr(line,"Est")!= NULL) {
            map.east = substr(line, 6);
        }
        else if (strstr(line,"Sud")!= NULL) {
            map.south = substr(line, 6);
        }
        else if (strstr(line, "Ouest") != NULL) {
            map.west = substr(line, 8);
        }
        else if (strstr(line,"Nord")!= NULL){
            map.north = substr(line, 7);
        }
        else if(line[0] >= 'A' && line[0] <= 'Z') {
            char * monsterLine = NULL;

            map.monsterClass[cptMonsterClass].name = line[0];

            getline(&monsterLine, &len, file);
            map.monsterClass[cptMonsterClass].max_hp = atoi(substr(monsterLine,5));
            free(monsterLine);

            monsterLine = NULL;
            getline(&monsterLine, &len, file);
            map.monsterClass[cptMonsterClass].attack = atoi(substr(monsterLine,8));
            free(monsterLine);

            monsterLine = NULL;
            getline(&monsterLine, &len, file);
            map.monsterClass[cptMonsterClass].defense = atoi(substr(monsterLine,9));

            cptMonsterClass++;

            free(line);
            line = NULL;
        }
    }
    fclose(file);

    int monsterIndex = 0;
    // Génération des monstres et de leurs caractéristiques en fonction de leur classe
    for(int i=0;i<MAX_SIZE;i++){
        for(int j=0;j<MAX_SIZE;j++){
            if(map.table[i][j] >= 'A' && map.table[i][j] <= 'Z') {
                for(int cpt=0;cpt<map.monsterClassCount;cpt++){
                    if(map.monsterClass[cpt].name == map.table[i][j]){
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].name    = 'A'+monsterIndex;
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].max_hp  = map.monsterClass[cpt].max_hp;
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].hp      = map.monsterClass[cpt].max_hp;
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].attack  = map.monsterClass[cpt].attack;
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].defense = map.monsterClass[cpt].defense;
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].pos.x   = i;
                        tabMonsters[nbRoomGenerated]->monsters[monsterIndex].pos.y   = j;
                        monsterIndex++;
                    }
                }
            }
        }
    }

    nbRoomGenerated++;
    
    return map;
}