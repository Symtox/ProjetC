#include "../../includes/raylib.h"
#include "./const.h"
#include "./utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX2(a,b) ((a>b)?(a):(b))
#define MAX4(a,b,c,d) MAX2(MAX2(a,b), MAX2(c,d))

#define lgoFilePath "./log.txt"

FILE * fp;



int fillWithRandAndWalls(int *** map) {
    *map = malloc(CHUNK_SIZE * sizeof(int *));
    for(int i = 0; i < CHUNK_SIZE; i++ ) {
        (*map)[i] = malloc(CHUNK_SIZE * sizeof(int));
        for(int j = 0; j < CHUNK_SIZE; j++) {
            if(i == 0 || j == 0 || i == CHUNK_SIZE-1 || j == CHUNK_SIZE-1) {
                (*map)[i][j] = 1;
            } else {
                (*map)[i][j] = GetRandomValue(-1, 0);;
            }
        }
    }
    return 0;
}

int logFile(const char * msg) {
    fwrite(msg, sizeof(char), strlen(msg), fp);
    return 0;
}

void initLogger() {
    fp = fopen("log.txt", "w");
    if(fp == NULL) {
        printf("null");
    }
}

void endLogger() {
    fclose(fp);
}

