#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils/const.h"

/*
 * Recupere "size" caracteres a partir de l'index dans la ligne
 */

char * getCharFromLine(char* line, int index, int size) {
    char * result = malloc(sizeof(char) * size * 2);
    for (int col = 0; col < size; col++) {
        result[col] = line[index + col];
    }
    return result;
}


int ** readCSV(char* filename, int x, int y) {
    int ** board = NULL;
    char ligne[1024];
    char * strippedLine;
    const char * separators = ";"; // séparateur de colonnes
    int row;
    srand(time(NULL));

    FILE * file = fopen(filename, "r");
    if (file == NULL){
        printf("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    board = malloc(sizeof(int*) * MAP_SIZE);
    for(int i=0; i < MAP_SIZE; i++){
        board[i] = malloc(sizeof(int) * MAP_SIZE);
    }

    for(int i = 0; i < 30 * y; i++) {
        fgets(ligne, 1024, file);
    }

    for(row = 30 * x; row < 30 * (x + 1); row++){//boucle sur chaque ligne du fichier (à partir de la première ligne)
        char * strToken;
        int col;

        fgets(ligne, 1024, file);

        if (row == 0) {
            memmove(ligne, ligne+3, strlen(ligne)-2); // supprime les 3 premiers caractères (BOM) de la première ligne
        }

        strippedLine = getCharFromLine(ligne, 30 * x * 2, MAP_SIZE * 2);

        strToken = strtok(strippedLine, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"
        for(col = 0; col < MAP_SIZE; col++) {
            board[row - 30 * x][col] = strToken[0] - '0';
            strToken = strtok(NULL, separators);
        }
        free(strippedLine);
    }
    fclose(file);
    return board;
}
