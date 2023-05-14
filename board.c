#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
char * getCharFromLine(char* line, int index, int size);

#define MAP_SIZE 30


void readCSV(char* filename, int x, int y){
    char board[MAP_SIZE][MAP_SIZE];
    char ligne[1024];
    char * strippedLine;
    const char * separators = ";"; // séparateur de colonnes
    int row;
    srand(time(NULL));

    FILE * file = fopen(filename, "r");
    if (file == NULL){
        printf("Erreur lors de l'ouverture du fichier");
        return;
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
        printf("%s\n\n", strippedLine);

        strToken = strtok(strippedLine, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"
        for(col = 0; col < MAP_SIZE; col++) {
            board[row - 30 * x][col] = strToken[0];
            strToken = strtok(NULL, separators);
        }
        free(strippedLine);
    }

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }

    fclose(file);
}

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


int main ()
{
//    randomBoard();
   readCSV("board.csv", 1, 1);
    return 0;
}