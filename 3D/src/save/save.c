#include "../../includes/raylib.h"
#include "../utils/const.h"
#include "../utils/utils.h"
#include "../board/board.h"
#include "../entities.h"
#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>




char * getCharFromLine(char* line, int index, int size) {
//    void * memcpy( void * destination, const void * source, size_t size );                       // Avant C99

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
        if (row == 0 && y == 0) {
            memmove(ligne, ligne+3, strlen(ligne)-2); // supprime les 3 premiers caractères (BOM) de la première ligne
        }


        strippedLine = getCharFromLine(ligne, 30 * x * 2, CHUNK_SIZE * 2);

        strToken = strtok(strippedLine, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"
        for(col = 0; col < CHUNK_SIZE; col++) {
            board[col][row - 30 * x] = strToken[0] - '0';
            strToken = strtok(NULL, separators);
        }
        free(strippedLine);
    }
    fclose(file);
    chunk->chunk = board;
    chunk->x = x;
    chunk->y = y;
}






chunkedMap_t loadMapFromBin(char * saveName) {
    chunkedMap_t map;
    return map;
}



void createSaveFromLevelFiles(char * saveName, char * levelName) {
    //TODO recupérer la map complete (sous forme de liste chainée de chunks)
    chunkedMap_t map = loadMap(levelName, 0, 0, MAP_CHUNK_WIDTH, MAP_CHUNK_HEIGHT, 0);
    player_t player = BASE_PLAYER;
    int fd = open(saveName, O_RDWR | O_CREAT, 0666);

    savePlayerContext(fd, player);
    for(int i = 0; i < MAP_CHUNK_WIDTH; i++) {
        for(int j = 0; j < MAP_CHUNK_HEIGHT; j++) {
            saveChunk(fd, map.chunks[i][j]);
        }
    }

    close(fd);
}

void savePlayerContext(int fd, player_t player) {
    write(fd, &player.inventory, sizeof(inventory_t));
    write(fd, &player.statistics, sizeof(statistics_t));
    write(fd, &player.physics, sizeof(playerPhysics_t));
    write(fd, &player.camera->target, sizeof(Vector3));
    write(fd, &player.camera->position, sizeof(Vector3));
}

void readPlayerContext(int fd, player_t * player) {
    read(fd, &player->inventory, sizeof(inventory_t));
    read(fd, &player->statistics, sizeof(statistics_t));
    read(fd, &player->physics, sizeof(playerPhysics_t));
    read(fd, &player->camera->target, sizeof(Vector3));
    read(fd, &player->camera->position, sizeof(Vector3));
}


off_t sizeofPlayerContext() {
    return sizeof(inventory_t) + sizeof(statistics_t) + sizeof(playerPhysics_t) + sizeof(Vector3) * 2;
}


void readIndex(int fd, index_t * index) {
    for(int i = 0; i < INDEX_SIZE; i++) {
        read(fd, index->chunkCoords[i], sizeof(int) * 2);
        read(fd, index->chunkFilePosition[i], sizeof(off_t) * 2);
    }
    read(fd, &index->next, sizeof(off_t));
}

void writeIndex(int fd, index_t index) {
    for(int i = 0; i < INDEX_SIZE; i++) {
        write(fd, index.chunkCoords[i], sizeof(int) * 2);
        write(fd, index.chunkFilePosition[i], sizeof(off_t) * 2);
    }
    write(fd, &index.next, sizeof(off_t));
}

void readChunk(int fd, chunk_t * chunk) {
    read(fd, &chunk->x, sizeof(int));
    read(fd, &chunk->y, sizeof(int));
    for(int i = 0; i < CHUNK_SIZE; i++) {
        read(fd, chunk->chunk[i], sizeof(int) * CHUNK_SIZE);
    }
}

void writeChunk(int fd, chunk_t chunk) {
    write(fd, &chunk.x, sizeof(int));
    write(fd, &chunk.y, sizeof(int));
    for(int i = 0; i < CHUNK_SIZE; i++) {
        write(fd, chunk.chunk[i], sizeof(int) * CHUNK_SIZE);
    }
}

//TODO
void saveChunk(int fd, chunk_t chunk) {
    index_t current_index;
    int chunkSize, oldChunkSize;
    off_t indexOffset = 0, lastindexOffset = 0, endPos, saveSize = 0;

    saveSize = lseek(fd, 0, SEEK_END);
    chunkSize = sizeofChunk();

    //Si la sauvegarde est vide
    if(saveSize < sizeofIndex()) {
        lseek(fd, 0, SEEK_SET);
        initIndex(&current_index);

        current_index.chunkFilePosition[0][0] = sizeofIndex();
        current_index.chunkFilePosition[0][1] = sizeofIndex() + sizeofChunk();

        current_index.chunkCoords[0][0] = chunk.x;
        current_index.chunkCoords[0][1] = chunk.y;

        writeIndex(fd, current_index);
        return;
    }



    while(indexOffset != -1) {
        initIndex(&current_index);
        lseek(fd, indexOffset, SEEK_SET);
        readIndex(fd, &current_index);
        for(int i = 0; i < INDEX_SIZE; i++) {

            //Si on trouve le chunk
            if(current_index.chunkCoords[i][0] == chunk.x && current_index.chunkCoords[i][1] == chunk.y) {

                oldChunkSize = current_index.chunkFilePosition[i][1] - current_index.chunkFilePosition[i][0];

                //Si le chunk peut être réécrit
                if(chunkSize <= oldChunkSize) {

                    //Victoire
                    lseek(fd, current_index.chunkFilePosition[i][0], SEEK_SET);
                    writeChunk(fd, chunk);

                    current_index.chunkFilePosition[i][1] = current_index.chunkFilePosition[i][0] + chunkSize;

                    lseek(fd, indexOffset, SEEK_SET);
                    writeIndex(fd, current_index);

                    return;

                } else {
                    //Si le chunk est écrit à la fin
                    endPos = lseek(fd, 0, SEEK_END);
                    writeChunk(fd, chunk);

                    current_index.chunkFilePosition[i][0] = endPos;
                    current_index.chunkFilePosition[i][1] = endPos + chunkSize;

                    //On actualise l'index avec la nouvelle adresse
                    lseek(fd, indexOffset, SEEK_SET);
                    writeIndex(fd, current_index);
                    return;

                }
            }

            //Si l'index n'est pas plein et le chunk pas dans la sauvegarde
            if(current_index.chunkCoords[i][0] == -1) {

                if((endPos = lseek(fd, 0, SEEK_END)) == -1 || i <= 0) {
                    return;
                }

                current_index.chunkCoords[i][0] = chunk.x;
                current_index.chunkCoords[i][1] = chunk.y;

                current_index.chunkFilePosition[i][0] = endPos;
                current_index.chunkFilePosition[i][1] = endPos + chunkSize;

                writeChunk(fd, chunk);
                lseek(fd, indexOffset, SEEK_SET);
                writeIndex(fd, current_index);
                return;
            }
        }
        lastindexOffset = indexOffset;
        indexOffset = current_index.next;
    }

    //Si le dernier index est plein
    current_index.next = lseek(fd, 0, SEEK_END);
    lseek(fd, lastindexOffset, SEEK_SET);
    writeIndex(fd, current_index);
    endPos=lseek(fd, 0, SEEK_END);

    initIndex(&current_index);
    writeIndex(fd, current_index);
    writeChunk(fd, chunk);

    current_index.chunkFilePosition[0][0] = endPos + sizeofIndex();
    current_index.chunkFilePosition[1][0] = endPos + sizeofChunk()+ sizeofIndex();

    current_index.chunkCoords[0][0] = chunk.x;
    current_index.chunkCoords[1][0] = chunk.y;

    lseek(fd, endPos, SEEK_SET);
    writeIndex(fd, current_index);
    return ;
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
    } else {
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
                if(fromSave) {
                    //loadChunkFromSave();
                } else {
                    loadChunk(filename, &map.chunks[i][j], ltcx+i, ltcy+j);
                }
            }
        }
    }
    return map;
}

int loadChunkFromSave(char * filename, chunk_t * chunk, int x, int y) {
    int fd = open(filename, O_RDONLY);
    //readChunk(fd, chunk, x, y);
    return 0;
}

int sizeofChunk() {
    return sizeof(int) * 2 + sizeof(int) * CHUNK_SIZE * CHUNK_SIZE;
}

int sizeofIndex() {
    return sizeof(int) * 2 * INDEX_SIZE + sizeof(off_t) * 2 * INDEX_SIZE + sizeof(int);
}

void loadCurrentMap(chunkedMap_t * map, Vector3 playerPos) {
    if (hasLeftChunk(*map, playerPos)) {
        map->centerX = (int)playerPos.x / CHUNK_SIZE;
        map->centerY = (int)playerPos.z / CHUNK_SIZE;
        freeMap(map);
        *map = loadMap("bin/levels/board/board.csv", map->centerX, map->centerY, map->width, map->height, 0);
    }
}


int isIndexFull(index_t index) {
    for(int i = 0; i < INDEX_SIZE; i++) {
        if(index.chunkCoords[i][0] == -1) {
            return 0;
        }
    }
    return 1;
}

int isChunkInIndex(index_t index, int x, int y) {
    for(int i = 0; i < INDEX_SIZE; i++) {
        if(index.chunkCoords[i][0] == x && index.chunkCoords[i][1] == y) {
            return 1;
        }
    }
    return 0;
}

void initIndex(index_t * index) {
    for(int i = 0; i < INDEX_SIZE; i++) {
        index->chunkCoords[i][0] = -1;
        index->chunkCoords[i][1] = -1;
        index->chunkFilePosition[i][0] = -1;
        index->chunkFilePosition[i][1] = -1;
    }
    index->next = -1;
}



