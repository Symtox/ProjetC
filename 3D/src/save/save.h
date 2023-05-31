//
// Created by 20015007@hq.corp.leroymerlin.com on 23/05/23.
//

#ifndef PROJECT_SAVE_H
#include <unistd.h>
#include "../board/board.h"
#define PROJECT_SAVE_H


typedef struct {
    int chunkCount;
    int ** chunkCoords;
    off_t ** chunkFilePosition;

}index_t;


void loadRandomChunk(chunk_t * chunk, int chunkX, int chunkY);
char * getCharFromLine(char* line, int index, int size);
void loadChunk(char* filename, chunk_t * chunk, int chunkX, int chunkY);
void savePlayerContext(int fd, player_t player);
void readPlayerContext(int fd, player_t * player);
off_t sizeofPlayerContext(); //TODO

void readIndex(int fd, index_t * index);
void writeIndex(int fd, index_t index);
size_t sizeofIndex(int indexSize);


size_t sizeofMonster();
void saveMonster(int fd, monster_t monster);
void readMonster(int fd, monster_t * monster);

size_t sizeofDoor();
void saveDoor(int fd, door_t door);
void readDoor(int fd, door_t * door);

size_t sizeofPotion();
void savePotion(int fd, potion_t potion);
void readPotion(int fd, potion_t * potion);

size_t sizeofKey();
void saveKey(int fd, DoorKey_t key);
void readKey(int fd, DoorKey_t * key);

size_t sizeofPowerUp();
void savePowerUp(int fd, powerUp_t powerUp);
void readPowerUp(int fd, powerUp_t * powerUp);

size_t sizeofChunkTXT(chunk_txt chunk);
void writeChunk(int fd, chunk_txt chunk);
void readChunk(int fd, chunk_t * chunk);

size_t sizeofStatistics();
void saveStatistics(int fd, statistics_t statistics);
void readStatistics(int fd, statistics_t * statistics);

size_t sizeofPhysics();
void savePhysics(int fd, playerPhysics_t physics);
void readPhysics(int fd, playerPhysics_t * physics);


void loadCurrentMap(int, chunkedMap_t * map, Vector3 playerPos);
chunkedMap_t loadMap(char * filename, int x, int y, int width, int height, int fromSave);

void loadChunkFromSave(int fd, chunk_t * chunk, int x, int y);
chunkedMap_t loadMapFromSave(int fd, int x, int y, int width, int height);
void loadPlayerFromSave(int fd, player_t * player);

#endif //PROJECT_SAVE_H
