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

/**
 * LEs méthodes suivante permettent de:
 * calculer la taille de chaque structure sauvegardée
 * sauvegarder chaque structure
 * lire chaque structure
 * Ces méthodes fonctionnes sur des fichiers binaire afin de garantir une taille de sauvegarde minimale
 */

void savePlayerContext(int fd, player_t player) {
    lseek(fd, sizeofMapContext(), SEEK_SET);
    write(fd, &player.keyCount, sizeof(int));
    saveStatistics(fd, player.statistics);
    savePhysics(fd, player.physics);
    write(fd, &player.camera->target, sizeof(Vector3));
    write(fd, &player.camera->position, sizeof(Vector3));
}

void readPlayerContext(int fd, player_t * player) {
    read(fd, &player->keyCount, sizeof(int));
    readStatistics(fd, &player->statistics);
    readPhysics(fd, &player->physics);
    read(fd, &player->camera->target, sizeof(Vector3));    
    logFile(TextFormat("READ : %d",read(fd, &player->camera->position, sizeof(Vector3))));
    
    logFile(TextFormat("TEST : %d", player->keyCount));
    logFile(TextFormat("TEST : %d", player->statistics.armor));
    logFile(TextFormat("TEST : %d", player->statistics.damage));
    logFile(TextFormat("TEST : %f", player->camera->position.x));
    logFile(TextFormat("TEST : %f", player->camera->position.y));    
    logFile(TextFormat("TEST : %f", player->camera->position.z));
}


off_t sizeofPlayerContext() {
    return sizeof(int) + sizeofStatistics() + sizeofPhysics() + sizeof(Vector3) * 2;
}


void readIndex(int fd, index_t * index) {
    lseek(fd, sizeofPlayerContext()+sizeofMapContext(), SEEK_SET);
    read(fd, &index->chunkCount, sizeof(int));
    logFile(TextFormat("CHUNK COUNT : %d", index->chunkCount));
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
    lseek(fd, sizeofPlayerContext()+sizeofMapContext(), SEEK_SET);
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
    res += sizeof(float) * 3;
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
    return sizeof(int) * 4;
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


chunkedMap_t loadMapFromSave(int fd, int x, int y, int width, int height, int mapWidth, int mapHeight) {
    chunkedMap_t map;

    int ltcx = MAX2(x - width / 2, 0);
    int ltcy = MAX2(y - height / 2, 0);

    ltcy = MIN2(ltcy, 0);
    ltcx = MIN2(ltcx, 0);

    map.centerX = x;
    map.centerY = y;

    map.height = height;
    map.width = width;

    map.maxY = mapHeight;
    map.maxX = mapWidth;

    map.chunks = malloc(sizeof(chunk_t *) * width);
    for (int i = 0; i < height; i++) {
        map.chunks[i] = malloc(sizeof(chunk_t) * height);
    }
    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            if (ltcx + i < 0 || ltcy + j < 0 || ltcx + i > mapWidth || ltcy + j > mapHeight) {
                map.chunks[i][j].x = -1;
                map.chunks[i][j].y = -1;
            } else {
                loadChunkFromSave(fd, &map.chunks[i][j], ltcx + i, ltcy + j);
            }
        }
    }

    return map;
}


size_t sizeofIndex(int indexSize) {
    return sizeof(int) * 2 * indexSize + sizeof(off_t) * 2 * indexSize + sizeof(int);
}

void loadCurrentMap(int fd, chunkedMap_t * map, Vector3 playerPos) {
    if (hasLeftChunk(*map, playerPos)) {
        saveMap(map, fd);
        map->centerX = (int)playerPos.x / CHUNK_SIZE;
        map->centerY = (int)playerPos.z / CHUNK_SIZE;
        freeMap(map);
        *map = loadMapFromSave(fd, map->centerX, map->centerY, map->width, map->height, map->maxX, map->maxY);
    }
}

void saveMap(chunkedMap_t * map, int fd) {
    saveMapContext(fd, *map);
    for(int i=0; i < map->height; i++) {
        for(int j=0; j < map->width; j++) {
            if(map->chunks[i][j].x == map->centerX && map->chunks[i][j].y == map->centerY) {
                saveChunk(map->chunks[i][j], fd);
            }
        }
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
    write(fd, &door.opened, sizeof(int));
}

void saveKey(int fd, DoorKey_t key) {
    write(fd, &key.position.x, sizeof(float));
    write(fd, &key.position.y, sizeof(float));
    write(fd, &key.position.z, sizeof(float));
    write(fd, &key.pickedUp, sizeof(int));
}


void saveStatistics(int fd, statistics_t statistics) {
    write(fd, &statistics.armor, sizeof(int));
    write(fd, &statistics.damage, sizeof(int));
    write(fd, &statistics.health, sizeof(int));
    write(fd, &statistics.maxHealth, sizeof(int));
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
    read(fd, &statistics->armor, sizeof(int));
    read(fd, &statistics->damage, sizeof(int));
    read(fd, &statistics->health, sizeof(int));
    read(fd, &statistics->maxHealth, sizeof(int));
}

void readMapContext(int fd, chunkedMap_t * context) {
    read(fd, &context->centerX, sizeof(int));
    read(fd, &context->centerY, sizeof(int));
    read(fd, &context->width, sizeof(int));
    read(fd, &context->height, sizeof(int));
    read(fd, &context->maxX, sizeof(int));
    read(fd, &context->maxY, sizeof(int));
}

void loadMapContext(int fd, chunkedMap_t * context) {
    lseek(fd, 0, SEEK_SET);
    readMapContext(fd, context);
}

void saveMapContext(int fd, chunkedMap_t context) {
    lseek(fd, 0, SEEK_SET);
    write(fd, &context.centerX, sizeof(int));
    write(fd, &context.centerY, sizeof(int));
    write(fd, &context.width, sizeof(int));
    write(fd, &context.height, sizeof(int));
    write(fd, &context.maxX, sizeof(int));
    write(fd, &context.maxY, sizeof(int));
}

void writeChunkTXT(int fd, chunk_txt chunk) {
    write(fd, &chunk.x, sizeof(int));
    write(fd, &chunk.y, sizeof(int));

    for(int i = 0; i < CHUNK_SIZE; i++) {
        for(int j = 0; j < MAX_Y; j++) {
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

void writeChunk(int fd, chunk_t chunk) {
    write(fd, &chunk.x, sizeof(int));
    write(fd, &chunk.y, sizeof(int));

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < MAX_Y; j++) {
            write(fd, chunk.chunk[i][j], sizeof(int) * CHUNK_SIZE);
        }
    }

    write(fd, &chunk.monsterCount, sizeof(int));
    for (int i = 0; i < chunk.monsterCount; i++) {
        saveMonster(fd, chunk.monsters[i]);
    }

    write(fd, &chunk.potionCount, sizeof(int));
    for (int i = 0; i < chunk.potionCount; i++) {
        savePotion(fd, chunk.potions[i]);
    }

    write(fd, &chunk.powerUpCount, sizeof(int));
    for (int i = 0; i < chunk.powerUpCount; i++) {
        savePowerUp(fd, chunk.powerUps[i]);
    }

    write(fd, &chunk.doorCount, sizeof(int));
    for (int i = 0; i < chunk.doorCount; i++) {
        saveDoor(fd, chunk.doors[i]);
    }

    write(fd, &chunk.keyCount, sizeof(int));
    for (int i = 0; i < chunk.keyCount; i++) {
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
    if(lseek(fd, sizeofMapContext(), SEEK_SET) == -1) {
        exit(EXIT_FAILURE);
    }
    readPlayerContext(fd, player);
}

int sizeofMapContext() {
    return 6 * sizeof(int);
}



void loadChunkFromSave(int fd, chunk_t * chunk, int x, int y) {
    index_t index;
    lseek(fd, sizeofPlayerContext() + sizeofMapContext(), SEEK_SET);
    readIndex(fd, &index);
    logFile(TextFormat("Searching for chunk %d %d %d ", x, y, index.chunkCount));

    for(int i = 0; i < index.chunkCount; i++) {
        logFile(TextFormat("Chunk %d %d", index.chunkCoords[i][0], index.chunkCoords[i][1]));
        if(index.chunkCoords[i][0] == x && index.chunkCoords[i][1] == y) {
            logFile(TextFormat("Chunk %d %d found", x, y));
            lseek(fd, index.chunkFilePosition[i][0], SEEK_SET);
            readChunk(fd, chunk);
            return;
        }
    }
    logFile(TextFormat("Chunk %d %d not found", x, y));
    chunk->x = -1;
    chunk->y = -1;
}



void freeIndex(index_t index) {
    for(int i = 0; i < index.chunkCount; i++) {
        free(index.chunkCoords[i]);
        free(index.chunkFilePosition[i]);
    }
    free(index.chunkCoords);
    free(index.chunkFilePosition);
}

void freeChunkTXT(chunk_txt chunk) {
    for(int i = 0; i < CHUNK_SIZE; i++) {
        for(int j = 0; j < MAX_Y; j++) {
            free(chunk.chunk[i][j]);
        }
        free(chunk.chunk[i]);
    }
    free(chunk.chunk);
    if(chunk.monsterCount > 0) {
        free(chunk.monsters);
    }
    if(chunk.potionCount > 0) {
        free(chunk.potions);
    }
    if(chunk.powerUpCount > 0) {
        free(chunk.powerUps);
    }
    if(chunk.doorCount > 0) {
        free(chunk.doors);
    }
    if(chunk.keyCount > 0) {
        free(chunk.keys);
    }
    free(chunk.east);
    free(chunk.west);
    free(chunk.north);
    free(chunk.south);
}

void saveChunk(chunk_t chunk, int fd) {
    index_t index;
    lseek(fd, sizeofPlayerContext() + sizeofMapContext(), SEEK_SET);
    readIndex(fd, &index);
    int chunkIndex = -1;
    for(int i = 0; i < index.chunkCount; i++) {
        if(index.chunkCoords[i][0] == chunk.x && index.chunkCoords[i][1] == chunk.y) {
            chunkIndex = i;
            break;
        }
    }
    if(chunkIndex == -1) {
        return;
    }
    lseek(fd, index.chunkFilePosition[chunkIndex][0], SEEK_SET);
    writeChunk(fd, chunk);
}