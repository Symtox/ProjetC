#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "../utils/utils.h"
#include <time.h>
#include "../utils/const.h"
#include "../../includes/raylib.h"
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


void loadChunk(char* filename, chunk_t * chunk, int x, int y) {
    int ** board = NULL;
    char ligne[1024];
    char * strippedLine;
    const char * separators = ","; // séparateur de colonnes
    int row;
    srand(time(NULL));


    FILE * file = fopen(filename, "r");
    if (file == NULL){
        printf("Erreur lors de l'ouverture du fichier");
        return;
    }

    board = malloc(sizeof(int*) * CHUNK_SIZE);
    for(int i=0; i < CHUNK_SIZE; i++){
        board[i] = malloc(sizeof(int) * CHUNK_SIZE);
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

        strippedLine = getCharFromLine(ligne, 30 * x * 2, CHUNK_SIZE * 2);

        strToken = strtok(strippedLine, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"
        for(col = 0; col < CHUNK_SIZE; col++) {
            board[row - 30 * x][col] = strToken[0] - '0';
            strToken = strtok(NULL, separators);
        }
        free(strippedLine);
    }
    fclose(file);
    chunk->chunk = board;
    chunk->x = x;
    chunk->y = y;
}

map_t loadMap(char * filename, int x, int y) {
    map_t map;

    x = MAX2(x, 0);
    y = MAX2(y, 0);

    map.centerX = x;
    map.centerY = y;


    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            //TODO test aussi si le chunk existe dans le fichier
            if(x-1+i < 0 || y-1+j < 0) {
                map.chunks[i][j].x = -1;
                map.chunks[i][j].y = -1;
            } else {
                loadChunk(filename, &map.chunks[i][j], x-1+i, y-1+j);
            }
        }
    }
    return map;
}

int hasLeftChunk(map_t map, Vector3 playerPos) {
    logFile(TextFormat("Current chunk: %d, %d\n", (int)playerPos.x / CHUNK_SIZE, (int)playerPos.z / CHUNK_SIZE));
    return (int)playerPos.x / CHUNK_SIZE != map.centerX || (int)playerPos.z / CHUNK_SIZE != map.centerY;
}

void loadCurrentMap(map_t * map, Vector3 playerPos) {
    if (hasLeftChunk(*map, playerPos)) {
        map->centerX = (int)playerPos.x / CHUNK_SIZE;
        map->centerY = (int)playerPos.z / CHUNK_SIZE;
        freeMap(map);
        *map = loadMap("./assets/board.csv", map->centerX, map->centerY);
    }
}

void freeMap(map_t * map) {

    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            if(map->chunks[i][j].x != -1) {
                freeChunk(&map->chunks[i][j]);
            }
        }
    }
}

void freeChunk(chunk_t * chunk) {
    for(int i = 0; i < CHUNK_SIZE; i++) {
        free(chunk->chunk[i]);
    }
    free(chunk->chunk);
}

