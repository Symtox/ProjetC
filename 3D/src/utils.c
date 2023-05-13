#include "../includes/raylib.h"
#include "./Const.h"
#include "./utils.h"
#include <stdlib.h>

#define MAX2(a,b) ((a>b)?(a):(b))
#define MAX4(a,b,c,d) MAX2(MAX2(a,b), MAX2(c,d))


int fillWithRandAndWalls(int *** map) {
    *map = malloc(MAP_SIZE * sizeof(int *));
    for(int i = 0; i < MAP_SIZE; i++ ) {
        (*map)[i] = malloc(MAP_SIZE * sizeof(int));
        for(int j = 0; j < MAP_SIZE; j++) {
            if(i == 0 || j == 0 || i == MAP_SIZE-1 || j == MAP_SIZE-1) {
                (*map)[i][j] = 1;
            } else {
                (*map)[i][j] = GetRandomValue(-1, 0);;
            }
        }
    }
    return 0;
}