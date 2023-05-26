#ifndef PROJECT_TILES
#define PROJECT_TILES

typedef enum tileTypes {
    GROUND = 0,
    WALL = 1,
    POWER_UP = 2,
}tileTypes_e;


int getHeightFromTileType(tileTypes_e type) {
    switch (type) {
        case GROUND:
            return 1;
        case WALL:
            return 4;
        case POWER_UP:
            return 2;
        default:
            return 1;
    }
}

#endif