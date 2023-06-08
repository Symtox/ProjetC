#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H
#include "../utils/const.h"
#include "../../includes/raylib.h"
#include "entities.h"

typedef struct {
    int opened;
    Vector3 position;
} door_t;

typedef struct {
    int pickedUp;
    Vector3 position;
} potion_t;

// Type de power up
typedef enum {
    ATTACK = 1,
    DEFENSE = 2,
    MAX_HP = 3
}powerUpType_e;

typedef struct {
    int pickedUp;
    powerUpType_e type;
    Vector3 position;
} powerUp_t;

typedef struct {
    int pickedUp;
    Vector3 position;
} DoorKey_t;

//UN chunk défini un troncon de la map global, soit un tableau de 30 x 30, les block en Y sont extrapolés du fichier txt
typedef struct {
    int x; // Coordonnées du chunk dans la map
    int y;
    int *** chunk;

    door_t * doors;
    int doorCount;

    powerUp_t * powerUps;
    int powerUpCount;

    potion_t * potions;
    int potionCount;

    monster_t * monsters;
    int monsterCount;

    DoorKey_t * keys;
    int keyCount;
} chunk_t;

/**
 * Structure permettant uniquement la lecture du fichier txt
 * Les données sont ensuite copiées dans la structure chunk_t
 */
typedef struct {
    int x;
    int y;
    int *** chunk;

    door_t * doors;
    int doorCount;

    powerUp_t * powerUps;
    int powerUpCount;

    potion_t * potions;
    int potionCount;

    monster_t * monsters;
    int monsterCount;

    DoorKey_t * keys;
    int keyCount;

    char * east;
    char * north;
    char * west;
    char * south;
} chunk_txt;

/**
 * Structure de map. Ceci ne représente pas la map globale
 */
typedef struct {
    int centerX; // Coordonées du chunk central
    int centerY;
    // Tableau de chunk de taille width x height
    chunk_t ** chunks;
    int width; // Taille du tableau, correspond à la "render distance", taille de rendu
    int height;
    int maxX; // Taille de la map globale (afin de corriger les problèmes de bordure)
    int maxY;
} chunkedMap_t;

void freeMap(chunkedMap_t * map);
void freeChunk(chunk_t * chunk);
int hasLeftChunk(chunkedMap_t, Vector3);
#endif