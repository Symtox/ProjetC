#include "../../includes/raylib.h"
#include "../utils/const.h"
#include "../utils/utils.h"
#include "../board/board.h"
#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



char * getCharFromLine(char* line, int index, int size) {
    char * result = malloc(sizeof(char) * size * 2);
    for (int col = 0; col < size; col++) {
        result[col] = line[index + col];
    }
    return result;
}


void loadChunk(char* filename, chunk_t * chunk, int chunkX, int chunkY) {
    int *** board = NULL;
    char line[1024];
    char * strippedLine;
    const char * separators = ","; // séparateur de colonnes
    int row;

    powerUp_t powerUps[40];
    door_t doors[40];
    monster_t monsters[40];
    DoorKey_t keys[40];

    FILE * file = fopen(filename, "r");
    if (file == NULL){
        logFile("Erreur lors de l'ouverture du fichier");
        return;
    }

    board = malloc(sizeof(int**) * CHUNK_SIZE);
    for(int x=0; x < CHUNK_SIZE; x++) {
        board[x] = malloc(sizeof(int*) * MAX_Y);
        for(int y=0; y < MAX_Y; y++) {
            board[x][y] = calloc(sizeof(int), CHUNK_SIZE);
        }
    }
    chunk->keyCount = 0;
    chunk->doorCount = 0;
    chunk->powerUpCount = 0;
    chunk->monsterCount = 0;

    for(int i = 0; i < 30 * chunkY; i++) {
        fgets(line, 1024, file);
    }



    for(row = 30 * chunkY; row < 30 * (chunkY + 1); row++){//boucle sur chaque ligne du fichier (à partir de la première ligne)
        char * strToken;
        int col;

        fgets(line, 1024, file);
        if (row == 0 && chunkY == 0) {
            //memmove(line, line+3, strlen(line)-2); // supprime les 3 premiers caractères (BOM) de la première ligne
        }


        strippedLine = getCharFromLine(line, 30 * chunkX * 2, CHUNK_SIZE * 2);

        strToken = strtok(strippedLine, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"

        for(col = 30 * chunkX; col < 30 * (chunkX + 1); col++) {
            board[col - 30 * chunkX][0][row - 30 * chunkY] = 1;
            switch (strToken[0] - '0') {
                case 1:
                    for(int i=0; i < WALL_HEIGHT; i++) {
                        board[col - 30 * chunkX][i][row - 30 * chunkY] = 1;
                    }
                    break;
                case 2:
                    powerUps[chunk->powerUpCount].pickedUp = 0;
                    powerUps[chunk->powerUpCount].position = (Vector3){col, 0, row};
                    chunk->powerUpCount++;
                    break;
                case 3:
                    for(int i=DOOR_HEIGHT + 1; i < WALL_HEIGHT; i++) {
                        board[col- 30 * chunkX][i][row - 30 * chunkY] = 1;
                    }
                    doors[chunk->doorCount].opened = 0;
                    doors[chunk->doorCount].rotation = 90;
                    doors[chunk->doorCount].position = (Vector3){col, 0, row};
                    chunk->doorCount++;
                    break;
                case 4:
                    keys[chunk->keyCount].pickedUp = 0;
                    keys[chunk->keyCount].position = (Vector3){(float)col, 0.0f, (float)row};
                    chunk->keyCount++;
                    break;

            }
            strToken = strtok(NULL, separators);
        }
        free(strippedLine);
    }
    fclose(file);


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
    chunk->chunk = board;
    chunk->x = chunkX;
    chunk->y = chunkY;
}



void savePlayerContext(int fd, player_t player) {
    write(fd, &player.keyCount, sizeof(int));
    saveStatistics(fd, player.statistics);
    savePhysics(fd, player.physics);
    write(fd, &player.camera->target, sizeof(Vector3));
    write(fd, &player.camera->position, sizeof(Vector3));
}

void readPlayerContext(int fd, player_t * player) {
    if(read(fd, &player->keyCount, sizeof(int)) == -1) {
        logFile("Erreur lors de la lecture du fichier");
        return;
    }
    readStatistics(fd, &player->statistics);
    readPhysics(fd, &player->physics);
    read(fd, &player->camera->target, sizeof(Vector3));
    read(fd, &player->camera->position, sizeof(Vector3));
}


off_t sizeofPlayerContext() {
    return sizeof(int) + sizeofStatistics() + sizeof(playerPhysics_t) + sizeof(Vector3) * 2;
}


void readIndex(int fd, index_t * index) {
    read(fd, &index->chunkCount, sizeof(int));
    index->chunkCoords = malloc(sizeof(int*) * index->chunkCount);
    index->chunkFilePosition = malloc(sizeof(off_t*) * index->chunkCount);

    for(int i = 0; i < index->chunkCount; i++) {
        index->chunkCoords[i] = malloc(sizeof(int) * 2);
        index->chunkFilePosition[i] = malloc(sizeof(off_t) * 2);
        read(fd, index->chunkFilePosition[i], sizeof(off_t) * 2);
        read(fd, index->chunkCoords[i], sizeof(int) * 2);
    }
}

void writeIndex(int fd, index_t index) {

    write(fd, &index.chunkCount, sizeof(int));
    for(int i = 0; i < index.chunkCount; i++) {
        write(fd, index.chunkFilePosition[i], sizeof(off_t) * 2);
        write(fd, index.chunkCoords[i], sizeof(int) * 2);
    }
}



size_t sizeofMonster() {
    size_t res = 0;
    res += sizeof(float) * 3;
    res += sizeofStatistics();
    res += sizeof(int);
    return res;
}

size_t sizeofDoor() {
    size_t res = 0;
    res += sizeof(float) * 4;
    res += sizeof(int);
    return res;
}

size_t sizeofKey() {
    size_t res = 0;
    res += sizeof(float) * 3;
    res += sizeof(int);
    return res;
}

size_t sizeofPowerUp() {
    size_t res = 0;
    res += sizeof(float) * 3;
    res += sizeof(int) * 2;
    return res;
}

size_t sizeofPotion() {
    size_t res = 0;
    res += sizeof(float) * 3;
    res += sizeof(int);
    return res;
}

size_t sizeofStatistics() {
    return sizeof(float) * 4;
}

size_t sizeofChunkTXT(chunk_txt chunk) {
    size_t res = 0;
    res += sizeof(int) * CHUNK_SIZE * CHUNK_SIZE * MAX_Y;
    res += 7 * sizeof(int);
    res += sizeofDoor() * chunk.doorCount;
    res += sizeofKey() * chunk.keyCount;
    res += sizeofPowerUp() * chunk.powerUpCount;
    res += sizeofMonster() * chunk.monsterCount;
    res += sizeofPotion() * chunk.potionCount;
    return res;
}

size_t sizeofPhysics() {
    return sizeof(float) * 2 + sizeof(int) * 5;
}



chunkedMap_t loadMap(char * filename, int x, int y, int width, int height, int fromSave) {
    chunkedMap_t map;

    int ltcx = MAX2(x - width/2, 0);
    int ltcy = MAX2(y - height/2, 0);
    ltcy = MIN2(ltcy, MAP_CHUNK_HEIGHT - height);
    ltcx = MIN2(ltcx, MAP_CHUNK_WIDTH - width);


    if(x - width/2 < 0) {
        ltcx = 0;
    } else if (ltcx + width >= MAP_CHUNK_WIDTH) {
        ltcx = MAP_CHUNK_WIDTH - width;
    } else if (x-width/2 >= 0) {
        ltcx = x - width/2;
    }



    if(y - height/2 < 0) {
        ltcy = 0;
    }


    map.centerX = x;
    map.centerY = y;

    map.height = height;
    map.width = width;

    map.chunks = malloc(sizeof(chunk_t*) * width);
    for(int i = 0; i < height; i++) {
        map.chunks[i] = malloc(sizeof(chunk_t) * height);
    }

    for(int i = 0; i < map.width; i++) {
        for(int j = 0; j < map.height; j++) {
            //TODO test aussi si le chunk existe dans le fichier
            if(ltcx+i < 0 || ltcy+j < 0 || ltcx + i > MAP_CHUNK_WIDTH || ltcy + j > MAP_CHUNK_HEIGHT) {
                map.chunks[i][j].x = -1;
                map.chunks[i][j].y = -1;
            } else {
                  //  loadRandomChunk(&map.chunks[i][j], ltcx+i, ltcy+j);
                    loadChunk(filename, &map.chunks[i][j], ltcx+i, ltcy+j);
            }
        }
    }
    return map;
}


chunkedMap_t loadMapFromSave(int fd, int x, int y, int width, int height) {
    chunkedMap_t map;

    int ltcx = MAX2(x - width / 2, 0);
    int ltcy = MAX2(y - height / 2, 0);
    ltcy = MIN2(ltcy, MAP_CHUNK_HEIGHT - height);
    ltcx = MIN2(ltcx, MAP_CHUNK_WIDTH - width);


    //TODO REMOVE
    if (x - width / 2 < 0) {
        ltcx = 0;
    } else if (ltcx + width >= MAP_CHUNK_WIDTH) {
        ltcx = MAP_CHUNK_WIDTH - width;
    } else if (x - width / 2 >= 0) {
        ltcx = x - width / 2;
    }


    if (y - height / 2 < 0) {
        ltcy = 0;
    }


    map.centerX = x;
    map.centerY = y;

    map.height = height;
    map.width = width;

    map.chunks = malloc(sizeof(chunk_t *) * width);
    for (int i = 0; i < height; i++) {
        map.chunks[i] = malloc(sizeof(chunk_t) * height);
    }

    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            if (ltcx + i < 0 || ltcy + j < 0 || ltcx + i > MAP_CHUNK_WIDTH || ltcy + j > MAP_CHUNK_HEIGHT) {
                map.chunks[i][j].x = -1;
                map.chunks[i][j].y = -1;
            } else {
                logFile(TextFormat("Loading chunk %d %d", ltcx + i, ltcy + j));
                loadChunkFromSave(fd, &map.chunks[i][j], ltcx + i, ltcy + j);
            }
        }
    }

    for(int i=0; i < map.width; i++) {
        for(int j=0; j < map.height; j++) {
            logFile(TextFormat("Chunk %d %d", map.chunks[i][j].x, map.chunks[i][j].y));
        }
    }
    return map;
}


void loadRandomChunk(chunk_t * chunk, int chunkX, int chunkY) {
    int *** board;
    board = malloc(sizeof(int**) * CHUNK_SIZE);
    for(int x=0; x < CHUNK_SIZE; x++) {
        board[x] = malloc(sizeof(int*) * MAX_Y);
        for(int y=0; y < MAX_Y; y++) {
            board[x][y] = calloc(sizeof(int), CHUNK_SIZE);
        }
    }

    for(int x=0; x < CHUNK_SIZE; x++) {
        for(int y=0; y < MAX_Y; y++) {

            for(int z=0; z < CHUNK_SIZE; z++) {
                if(y > 3) {
                    board[x][y][z] = 0;
                    continue;
                }
                board[x][y][z] = rand() % 2;
            }
        }
    }
    chunk->x = chunkX;
    chunk->y = chunkY;
    chunk->chunk = board;
}


size_t sizeofIndex(int indexSize) {
    return sizeof(int) * 2 * indexSize + sizeof(off_t) * 2 * indexSize + sizeof(int);
}

void loadCurrentMap(int fd, chunkedMap_t * map, Vector3 playerPos) {
    if (hasLeftChunk(*map, playerPos)) {
        map->centerX = (int)playerPos.x / CHUNK_SIZE;
        map->centerY = (int)playerPos.z / CHUNK_SIZE;
        freeMap(map);
        *map = loadMapFromSave(fd, map->centerX, map->centerY, map->width, map->height);
    }
}


void saveMonster(int fd, monster_t monster) {
    write(fd, &monster.position.x, sizeof(float));
    write(fd, &monster.position.y, sizeof(float));
    write(fd, &monster.position.z, sizeof(float));
    write(fd, &monster.isDead, sizeof(int));
    saveStatistics(fd, monster.statistics);
}

void savePotion(int fd, potion_t potion) {
    write(fd, &potion.position.x, sizeof(float));
    write(fd, &potion.position.y, sizeof(float));
    write(fd, &potion.position.z, sizeof(float));
    write(fd, &potion.pickedUp, sizeof(int));
}

void savePowerUp(int fd, powerUp_t powerUp) {
    write(fd, &powerUp.position.x, sizeof(float));
    write(fd, &powerUp.position.y, sizeof(float));
    write(fd, &powerUp.position.z, sizeof(float));
    write(fd, &powerUp.pickedUp, sizeof(int));
    write(fd, &powerUp.type, sizeof(int));
}

void saveDoor(int fd, door_t door) {
    write(fd, &door.position.x, sizeof(float));
    write(fd, &door.position.y, sizeof(float));
    write(fd, &door.position.z, sizeof(float));
    write(fd, &door.rotation, sizeof(float));
    write(fd, &door.opened, sizeof(int));
}

void saveKey(int fd, DoorKey_t key) {
    write(fd, &key.position.x, sizeof(float));
    write(fd, &key.position.y, sizeof(float));
    write(fd, &key.position.z, sizeof(float));
    write(fd, &key.pickedUp, sizeof(int));
}


void saveStatistics(int fd, statistics_t statistics) {
    write(fd, &statistics.armor, sizeof(float));
    write(fd, &statistics.damage, sizeof(float));
    write(fd, &statistics.health, sizeof(float));
    write(fd, &statistics.maxHealth, sizeof(float));
}

void savePhysics(int fd, playerPhysics_t physics) {
    write(fd, &physics.isJumping, sizeof(int));
    write(fd, &physics.isFalling, sizeof(int));
    write(fd, &physics.jumpTime, sizeof(int));
    write(fd, &physics.fallTime, sizeof(int));
    write(fd, &physics.jumpingSpeed, sizeof(float));
    write(fd, &physics.fallingSpeed, sizeof(float));
    write(fd, &physics.noclip, sizeof(int));
}

void readPhysics(int fd, playerPhysics_t * physics) {
    read(fd, &physics->isJumping, sizeof(int));
    read(fd, &physics->isFalling, sizeof(int));
    read(fd, &physics->jumpTime, sizeof(int));
    read(fd, &physics->fallTime, sizeof(int));
    read(fd, &physics->jumpingSpeed, sizeof(float));
    read(fd, &physics->fallingSpeed, sizeof(float));
    read(fd, &physics->noclip, sizeof(int));
}

void readStatistics(int fd, statistics_t * statistics) {
    read(fd, &statistics->armor, sizeof(float));
    read(fd, &statistics->damage, sizeof(float));
    read(fd, &statistics->health, sizeof(float));
    read(fd, &statistics->maxHealth, sizeof(float));
}

void writeChunk(int fd, chunk_txt chunk) {
    write(fd, &chunk.x, sizeof(int));
    write(fd, &chunk.y, sizeof(int));

    for(int i = 0; i < CHUNK_SIZE; i++) {
        for(int j = 0; j < MAX_Y; j++) {
            for(int k = 0; k < CHUNK_SIZE; k++) {
                //logFile(TextFormat("Writing %d %d %d %d", i, j, k, chunk.chunk[i][j][k]));
            }
            write(fd, chunk.chunk[i][j], sizeof(int) * CHUNK_SIZE);
        }
    }

    write(fd, &chunk.monsterCount, sizeof(int));
    for(int i = 0; i < chunk.monsterCount; i++) {
        saveMonster(fd, chunk.monsters[i]);
    }

    write(fd, &chunk.potionCount, sizeof(int));
    for(int i = 0; i < chunk.potionCount; i++) {
        savePotion(fd, chunk.potions[i]);
    }

    write(fd, &chunk.powerUpCount, sizeof(int));
    for(int i = 0; i < chunk.powerUpCount; i++) {
        savePowerUp(fd, chunk.powerUps[i]);
    }

    write(fd, &chunk.doorCount, sizeof(int));
    for(int i = 0; i < chunk.doorCount; i++) {
        saveDoor(fd, chunk.doors[i]);
    }

    write(fd, &chunk.keyCount, sizeof(int));
    for(int i = 0; i < chunk.keyCount; i++) {
        saveKey(fd, chunk.keys[i]);
    }
}


void readMonster(int fd, monster_t * monster) {
    read(fd, &monster->position.x, sizeof(float));
    read(fd, &monster->position.y, sizeof(float));
    read(fd, &monster->position.z, sizeof(float));
    read(fd, &monster->isDead, sizeof(float));
    readStatistics(fd, &monster->statistics);
}

void readPotion(int fd, potion_t * potion) {
    read(fd, &potion->position.x, sizeof(float));
    read(fd, &potion->position.y, sizeof(float));
    read(fd, &potion->position.z, sizeof(float));
    read(fd, &potion->pickedUp, sizeof(int));
}

void readPowerUp(int fd, powerUp_t * powerUp) {
    read(fd, &powerUp->position.x, sizeof(float));
    read(fd, &powerUp->position.y, sizeof(float));
    read(fd, &powerUp->position.z, sizeof(float));
    read(fd, &powerUp->pickedUp, sizeof(int));
    read(fd, &powerUp->type, sizeof(int));
}

void readDoor(int fd, door_t * door) {
    read(fd, &door->position.x, sizeof(float));
    read(fd, &door->position.y, sizeof(float));
    read(fd, &door->position.z, sizeof(float));
    read(fd, &door->rotation, sizeof(float));
    read(fd, &door->opened, sizeof(int));
}

void readKey(int fd, DoorKey_t * key) {
    read(fd, &key->position.x, sizeof(float));
    read(fd, &key->position.y, sizeof(float));
    read(fd, &key->position.z, sizeof(float));
    read(fd, &key->pickedUp, sizeof(int));
}

void readChunk(int fd, chunk_t * chunk) {
    read(fd, &chunk->x, sizeof(int));
    read(fd, &chunk->y, sizeof(int));

    chunk->chunk = malloc(sizeof(int**) * CHUNK_SIZE);
    for(int i = 0; i < CHUNK_SIZE; i++) {
        chunk->chunk[i] = malloc(sizeof(int*) * MAX_Y);
        for(int j = 0; j < MAX_Y; j++) {
            chunk->chunk[i][j] = calloc(sizeof(int), CHUNK_SIZE);
            read(fd, chunk->chunk[i][j], sizeof(int) * CHUNK_SIZE);
        }
    }



    logFile("Chunk %d %d loaded");


    read(fd, &chunk->monsterCount, sizeof(int));
    chunk->monsters = malloc(sizeof(monster_t) * chunk->monsterCount);
    for(int i = 0; i < chunk->monsterCount; i++) {
        readMonster(fd, &chunk->monsters[i]);
    }

    read(fd, &chunk->potionCount, sizeof(int));
    chunk->potions = malloc(sizeof(potion_t) * chunk->potionCount);
    for(int i = 0; i < chunk->potionCount; i++) {
        readPotion(fd, &chunk->potions[i]);
    }

    read(fd, &chunk->powerUpCount, sizeof(int));
    chunk->powerUps = malloc(sizeof(powerUp_t) * chunk->powerUpCount);
    for(int i = 0; i < chunk->powerUpCount; i++) {
        readPowerUp(fd, &chunk->powerUps[i]);
    }

    read(fd, &chunk->doorCount, sizeof(int));
    chunk->doors = malloc(sizeof(door_t) * chunk->doorCount);
    for(int i = 0; i < chunk->doorCount; i++) {
        readDoor(fd, &chunk->doors[i]);
    }

    read(fd, &chunk->keyCount, sizeof(int));
    chunk->keys = malloc(sizeof(DoorKey_t) * chunk->keyCount);
    for(int i = 0; i < chunk->keyCount; i++) {
        readKey(fd, &chunk->keys[i]);
    }

}

void loadPlayerFromSave(int fd, player_t * player) {
    if(lseek(fd, 0, SEEK_SET) == -1) {
        exit(EXIT_FAILURE);
    }
    readPlayerContext(fd, player);
}

void loadChunkFromSave(int fd, chunk_t * chunk, int x, int y) {
    index_t index;
    lseek(fd, sizeofPlayerContext(), SEEK_SET);
    readIndex(fd, &index);
    logFile(TextFormat("index.chunkCount: %d, looking for (%d, %d)", index.chunkCount, x, y));
    for(int i = 0; i < index.chunkCount; i++) {
        if(index.chunkCoords[i][0] == x && index.chunkCoords[i][1] == y) {
            logFile("Found chunk");
            lseek(fd, index.chunkFilePosition[i][0], SEEK_SET);
            readChunk(fd, chunk);
            return;
        }
    }
    chunk->x = -1;
    chunk->y = -1;
}




