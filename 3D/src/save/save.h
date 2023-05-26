//
// Created by 20015007@hq.corp.leroymerlin.com on 23/05/23.
//

#ifndef PROJECT_SAVE_H
#include <unistd.h>
#include "../board/board.h"
#define PROJECT_SAVE_H
#define INDEX_SIZE 10


typedef struct {
    int chunkCoords[INDEX_SIZE][2];
    off_t chunkFilePosition[INDEX_SIZE][2];
    off_t next;
}index_t;


char * getCharFromLine(char*, int, int);
void loadChunk(char*, chunk_t *, int, int);
chunkedMap_t loadMap(char *, int, int, int, int, int);
void loadCurrentMap(chunkedMap_t *, Vector3);

void createSaveFromLevelFiles(char * , char * );


void saveChunk(int, chunk_t);
void savePlayerContext(int fd, player_t player);

void readIndex(int, index_t *);
void writeIndex(int, index_t);



void initIndex(index_t *);

int sizeofChunk();
int sizeofIndex();




#endif //PROJECT_SAVE_H
