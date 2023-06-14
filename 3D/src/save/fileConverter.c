
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../board/entities.h"
#include "../board/board.h"
#include "save.h"
#include "../utils/utils.h"
#include "fileConverter.h"
#include <wchar.h>
#include <locale.h>
/**
 * Substring depuis index pos
 * @param src
 * @param pos
 * @return
 */
char * substr(char *src, int pos) {
    if(pos >= strlen(src) || pos < 0 || src == NULL || src[pos] == '\0' || src[pos] == '\n' || src[pos] == '\r') {
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

/**
 * Lis un chunk depuis un fichier
 * @param chunk
 * @param path
 */
void loadChunkFromTXT(chunk_txt * chunk, char* path, player_t * player, int x, int y) {
    FILE *file;

    setlocale(LC_ALL, "");
    chunk->monsterCount = 0;
    chunk->doorCount = 0;
    chunk->keyCount = 0;
    chunk->powerUpCount = 0;
    chunk->potionCount = 0;
    chunk->x = x;
    chunk->y = y;

    //Allocation du chunk
    chunk->chunk = malloc(sizeof(int**) * CHUNK_SIZE);
    for(int i=0; i < CHUNK_SIZE; i++) {
        chunk->chunk[i] = malloc(sizeof(int*) * MAX_Y);
        for(int j=0; j < MAX_Y; j++) {
            chunk->chunk[i][j] = calloc(CHUNK_SIZE, sizeof(int));
        }
    }

    //Buffers
    potion_t potion[60] = {0};
    monster_t monsters[60] = {0};
    door_t doors[60] = {0};
    DoorKey_t keys[60] = {0};
    powerUp_t powerUps[60] = {0};

    //Ouverture du fichier
    file = fopen(path, "r");
    if (file == NULL) {
        logFile("Error opening file");
        return;
    }

    //Lecture du board
    for(int i = CHUNK_SIZE - 1; i >= 0; i--) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            char currChar = fgetc(file);
            switch (currChar) {
                case '#':
                    for (int k = 0; k < WALL_HEIGHT; k++) {
                        chunk->chunk[i][k][j] = rand()% 3 +1; // Random la texture du mur
                    }
                    break;

                case -62: // §
                    fseek(file, 1, SEEK_CUR); // On avance d'un char
                    potion[chunk->powerUpCount].pickedUp = 0;
                    potion[chunk->powerUpCount].position = (Vector3) {i, 0, j};
                    chunk->potionCount++;
                    break;

                case 'o':
                    //Ajout de blocs au dessus de la porte
                    for (int k = DOOR_HEIGHT + 1; k < WALL_HEIGHT; k++) {
                        chunk->chunk[i][k][j] = 1;
                    }

                    doors[chunk->doorCount].opened = 0;
                    doors[chunk->doorCount].position = (Vector3) {i, 0, j};
                    chunk->doorCount++;
                    break;
                case '+':
                    player->camera->position = (Vector3) {i, 0, j};
                    player->chunkX = x;
                    player->chunkY = y;
                case '!':
                    keys[chunk->keyCount].pickedUp = 0;
                    keys[chunk->keyCount].position = (Vector3){i, 0.0f, j};
                    chunk->keyCount++;
                    break;
                case '1':
                case '2':
                case '3':
                    powerUps[chunk->powerUpCount].pickedUp = 0;
                    powerUps[chunk->powerUpCount].position = (Vector3){i, 0.0f, j};
                    powerUps[chunk->powerUpCount].type = currChar - '0';
                    chunk->powerUpCount++;
                    break;
            }

            if(currChar >= 'A' && currChar <= 'Z' ) {
                monsters[chunk->monsterCount].position = (Vector3) {i, 0, j};
                monsters[chunk->monsterCount].class = currChar;
                chunk->monsterCount++;
            }
        }
        fgetc(file);
    }

    //fgetwc(file);

    char * line = (char *) malloc( sizeof(char) * 100 );

    chunk->east = NULL;
    chunk->south = NULL;
    chunk->west = NULL;
    chunk->north = NULL;

    while (fgets(line, 30, file) != NULL) {
        if (strstr(line,"Est")!= NULL) {
            chunk->east = substr(line, 6);
        }
        else if (strstr(line,"Sud")!= NULL) {
            chunk->south = substr(line, 6);
        }
        else if (strstr(line, "Ouest") != NULL) {
            chunk->west = substr(line, 8);
        }
        else if (strstr(line,"Nord")!= NULL){
            chunk->north = substr(line, 7);
        }
        else if(line[0] >= 'A' && line[0] <= 'Z') {
            statistics_t stats;
            char monsterLine[30] = {0};

            fgets(monsterLine, 30, file);
            stats.health = atoi(substr(monsterLine,5));
            stats.maxHealth = stats.health;

            fgets(monsterLine, 30, file);
            stats.damage = atoi(substr(monsterLine,8));

            fgets(monsterLine, 30, file);
            stats.armor = atoi(substr(monsterLine,9));

            for(int i = 0; i < chunk->monsterCount; i++) {
                if(monsters[i].class == line[0]) {
                    monsters[i].statistics = stats;
                }
            }
        }
    }
    //On alloue des tableau de taille optimal pour le chunk a partir des buffers TODO: memcpy
    if(chunk->doorCount > 0) {
        chunk->doors = malloc(sizeof(door_t) * chunk->doorCount);
        for(int i=0; i < chunk->doorCount; i++) {
            chunk->doors[i] = doors[i];
        }
    }
    if(chunk->keyCount > 0) {
        chunk->keys = malloc(sizeof(DoorKey_t) * chunk->keyCount);
        for(int i=0; i < chunk->keyCount; i++) {
            chunk->keys[i] = keys[i];
        }
    }
    if(chunk->powerUpCount > 0) {
        chunk->powerUps = malloc(sizeof(powerUp_t) * chunk->powerUpCount);
        for(int i=0; i < chunk->powerUpCount; i++) {
            chunk->powerUps[i] = powerUps[i];
        }
    }
    if(chunk->monsterCount > 0) {
        chunk->monsters = malloc(sizeof(monster_t) * chunk->monsterCount);
        for(int i=0; i < chunk->monsterCount; i++) {
            chunk->monsters[i] = monsters[i];
        }
    }
    if(chunk->potionCount > 0) {
        chunk->potions = malloc(sizeof(potion_t) * chunk->potionCount);
        for(int i=0; i < chunk->potionCount; i++) {
            chunk->potions[i] = potion[i];
        }
    }

    fclose(file);
}

/**
 * Concatenation de path et du nom de fichier
 * @param path
 * @param filename
 * @return
 */
char * concatPath(char * path, char * filename) {
    char * result = malloc(strlen(path) + strlen(filename) + 1);
    strncpy(result, path, strlen(path));
    result[strlen(path)] = '\0';
    strncat(result, filename, strlen(filename) + 1);
    return result;
}

//Global pour + de simplicité
chunk_txt chunkBuffer[MAX_CHUNK_COUNT] = {0};
int chunkCount = 0;

//Verifie si un couple de coordonné est déjà en mémoire
int isChunkInBuffer(int x, int y) {
    for(int i = 0; i < chunkCount; i++) {
        if(chunkBuffer[i].x == x && chunkBuffer[i].y == y) {
            return 1;
        }
    }
    return 0;
}

/**
 * Charge TOUT les chunk accessible depuis un certain chunk, donc en théorie la map entière (Voir createSaveFromLevelFiles)
 */
void createSaveFromLevelFilesR(char * path, char * filename, player_t * player, int x, int y) {
    int currentChunkNo = chunkCount; // On stock le numéro de chunk courant (Obligatoire car la valeur globale peut changer a cause des appel réccursif)
    if(isChunkInBuffer(x, y) || filename == NULL) { // Si aucun chunk ou chunk déjà load
        return;
    }
    char * fullPath = concatPath(path, filename);
    loadChunkFromTXT(&chunkBuffer[currentChunkNo], fullPath, player, x, y); // Chargement du chunk

    //On ajoute le chunk au tableau global
    chunkCount++;

    //Appels réccursif pour charger les chunk adjacent
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].east, player, x, y+1);
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].south, player, x-1, y);
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].west, player, x, y-1);
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].north, player, x+1, y);

    free(fullPath); // Warning par mon IDE mais le free semble correct
}

/**
 * Charge TOUTE une map
 * @param path
 * @param filename
 * @param fd
 */
void createSaveFromLevelFiles(char * path, char * filename, int fd) {
    index_t index; // Structure de sauvegarde
    //layer_t player = BASE_PLAYER; // Joueur avec stat de base
    player_t player = {0}; // Joueur avec stat de base
    player.statistics.health = 10;
    player.statistics.maxHealth = 10;
    player.statistics.damage = 2;
    player.statistics.armor = 1;
    player.physics.fallingSpeed = FALL_BASE_SPEED;
    player.physics.jumpingSpeed = JUMP_BASE_SPEED;
    //Camera camera = BASE_CAMERA;
    Camera camera = {0};
    camera.position = (Vector3){15.0f, 10.0f, 15.0f};
    camera.fovy = 90.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    player.camera = &camera;

    Vector2 min = {0,0}; //Coordonnées minimal d'un chunk (Afin de normaliser les chunk)
    Vector2 max = {0,0}; //Coordonnées maximal d'un chunk (Taille de la map)

    chunkCount = 0;
    chunkedMap_t mapContext = {0};

    //Chargement de la map
    createSaveFromLevelFilesR(path, filename, &player, 0, 0);

    //Calcul des coord min et max
    for(int i = 0; i < chunkCount; i++) {
        if(chunkBuffer[i].x < min.x) {
            min.x = chunkBuffer[i].x;
        }
        if(chunkBuffer[i].y < min.y) {
            min.y = chunkBuffer[i].y;
        }
        if(chunkBuffer[i].x > max.x) {
            max.x = chunkBuffer[i].x;
        }
        if(chunkBuffer[i].y > max.y) {
            max.y = chunkBuffer[i].y;
        }
    }

    //Normalisation des coordonnées de chunk (Aucun chunk ne peut avoir de coordonnées négative)
    for(int i = 0; i < chunkCount; i++) {
        chunkBuffer[i].x -= min.x;
        chunkBuffer[i].y -= min.y;
    }
    player.chunkX -= min.x;
    player.chunkY -= min.y;
    player.camera->position.x += player.chunkX * CHUNK_SIZE;
    player.camera->position.z += player.chunkY * CHUNK_SIZE;

    if(fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    //INitialisation de la structure de stockage
    index.chunkCount = chunkCount;
    index.chunkCoords = malloc(sizeof(Vector2) * chunkCount);
    index.chunkFilePosition = malloc(sizeof(int*) * chunkCount);

    //pos est la position actuel ou on écrit de le fichier de sauvegarde
    //Ici on commence a écrire les chunk seulement apres avoir écrit les info de la map, du joueur et de l'index
    off_t pos = sizeofIndex(chunkCount) + sizeofPlayerContext() + sizeofMapContext();
    //ON sauvegarde dans l'index pour chaque chunk, la position ou il commence et ou il fini dans le fichier de sauvegarde
    for(int i = 0; i < chunkCount; i++) {

        index.chunkCoords[i] = malloc(sizeof(int) * 2);
        index.chunkCoords[i][0] = chunkBuffer[i].x;
        index.chunkCoords[i][1] = chunkBuffer[i].y;

        index.chunkFilePosition[i] = malloc(sizeof(int) * 2);
        index.chunkFilePosition[i][0] = pos;
        pos += sizeofChunkTXT(chunkBuffer[i]);
        index.chunkFilePosition[i][1] = pos;
    }

    //Info de la carte
    mapContext.centerY = 0;
    mapContext.centerX = 0;
    mapContext.width = 2;
    mapContext.height = 2;
    mapContext.maxX = max.x - min.x + 1;
    mapContext.maxY = max.y - min.y + 1;

    //Sauvegarde de la map, du joueur et de l'index
    saveMapContext(fd, mapContext);
    savePlayerContext(fd, player);
    writeIndex(fd, index);

    for(int i = 0; i < chunkCount; i++) {
        writeChunkTXT(fd, chunkBuffer[i]);
        freeChunkTXT(chunkBuffer[i]);
    }
    freeIndex(index);
}