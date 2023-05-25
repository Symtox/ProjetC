#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>
#include "./entities/entities.h"

//parcourir nb monstre, malloc le nb de monstre tab ert ensuite tu cpt pour lire les lignes
int isMonsterFound(char * monsters, char monster) {
    for(int i = 0; i < 26; i++) {
        if(monsters[i] == monster) {
            return 1;
        }
    }
    return 0;
}


map_t readTXT(){
    FILE *file;
    map_t map = {0};
    char caractere;
    int i = 0;
    int j = 0;
    char monsterFound[26] = {0};

    map.monsterCount = 0;
    map.monsterClassCount = 0;
 

    // Ouvrir le fichier en mode lecture
    file = fopen("./entities/niveau1.txt", "r");

    // Vérifier si le fichier est ouvert avec succès
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
    }

    // Lire et afficher le contenu du fichier caractère par caractère
    for(i=0; i < MAX_SIZE; i++) {
        for (j = 0; (caractere = fgetc(file)) != EOF && j < MAX_SIZE; j++) {
            if(caractere >= 'A' && caractere <= 'Z' ) {
                if(!isMonsterFound(monsterFound, caractere)) {
                    map.monsterClassCount++;
                }
                monsterFound[map.monsterCount] = caractere;
                map.monsterCount++;
            }
            map.table.plateau[i][j] = caractere;
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



int readMonster(monster_t * monster, FILE* file) {
    char* ligne = NULL;
    int longueur = 0;
    int tailleLigne = 100;
    getline(&ligne, &longueur, file);
    monster->hp = atoi(substr(ligne,5));

    getline(&ligne, &longueur, file);
    monster->attack = atoi(substr(ligne,8));

    getline(&ligne, &longueur, file);
    monster->defense = atoi(substr(ligne,8));



}


map_t initTab() {
    FILE *file;
    char* ligne = NULL;
    int longueur = 0;
    int tailleLigne = 100;
    file = fopen("./entities/niveau1.txt", "r");
    map_t map = readTXT();
    map.actual = "niveau1.txt";

    int monsterIndex = 0;

//mettre un monstre en fonction de sa classe dans le malloc monsters
//relire toute la map pour quand jr trouve A B C j insere dans le tab la classe
    map.monsters = (monster_t*)malloc(sizeof(monster_t) * map.monsterCount);
    map.monsterClass = (monster_t*)malloc(sizeof(monster_t) * map.monsterClassCount);
    


    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
    }

    while ((tailleLigne = getline(&ligne, &longueur, file)) != -1) {

        if (strstr(ligne,"Est")!= NULL){
            char * mapName = substr(ligne, 6);
            map.est = mapName;
        }
        else if (strstr(ligne,"Sud")!= NULL){
            char * mapName = substr(ligne, 6);
            map.sud = mapName;
        }
        else if (strstr(ligne, "Ouest") != NULL) {
            //printf("longueur : %d\n",longueur);
            char * mapName = substr(ligne, 8);
            map.ouest = mapName;
        }
        else if (strstr(ligne,"Nord")!= NULL){
            char * mapName = substr(ligne, 7);
            map.nord = mapName;
        }
        else if(ligne[0] >= 'A' && ligne[0] <= 'Z') {
            printf("Reading monster: %d %c\n", monsterIndex, ligne[0]);
            map.monsterClass[monsterIndex].name = (char)ligne[0];
            readMonster(&map.monsterClass[monsterIndex], file);
            monsterIndex++;
        }
    }   
            



    free(ligne);
    fclose(file);
    
    return map;
}




int printTab(table_t tab){
    int i = 0;
    int j = 0;
    for(i=0; i < MAX_SIZE; i++){
        for (j = 0; j < MAX_SIZE; j++) {
            printf("%c", tab.plateau[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int printMap(map_t map) {
    printTab(map.table);
    printf("Est : %s\n", map.est);
    printf("Ouest : %s\n", map.ouest);
    printf("Nord : %s\n", map.nord);
    printf("Sud : %s\n", map.sud);
    printf("Nombre de monstre : %d\n", map.monsterCount);
    printf("Nombre de classe de monstre : %d\n", map.monsterClassCount);
    for(int i = 0; i < map.monsterClassCount; i++) {
        printf("Monstre %d : \n", i);
        printf("Nom : %c\n", map.monsterClass[i].name);
        printf("HP : %d\n", map.monsterClass[i].hp);
        printf("Attack : %d\n", map.monsterClass[i].attack);
        printf("Defense : %d\n", map.monsterClass[i].defense);
    }

    return 0;
}


int main() {
    //table_t * tab = readTXT();
    
    printMap(initTab());
    //printTab(tab);
    return 0;

}
