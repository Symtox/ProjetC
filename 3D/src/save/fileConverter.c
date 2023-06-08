
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
ssize_t getLineFromText(char **lineptr, FILE *stream) {
    wchar_t buf[100] = {0};
    int length = 0;
    int c = 1;

    while(c != '\n' && c != EOF) {
        c = fgetc(stream);
        buf[length] = c;
        length += 1;
    }
    buf[length] = '\0';
    *lineptr = malloc(sizeof(char) * length);
    for(int i=0; i < length; i++) {
        (*lineptr)[i] = (char)buf[i];
    }
    return c == EOF ? -1 : length;
}

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

void loadChunkFromTXT(chunk_txt * chunk, char* path) {
    FILE *file;

    setlocale(LC_ALL, "");
    logFile(TextFormat("Loading chunk from %s", path));
    chunk->monsterCount = 0;
    chunk->doorCount = 0;
    chunk->keyCount = 0;
    chunk->powerUpCount = 0;
    chunk->potionCount = 0;

    chunk->chunk = malloc(sizeof(int**) * CHUNK_SIZE);
    for(int i=0; i < CHUNK_SIZE; i++) {
        chunk->chunk[i] = malloc(sizeof(int*) * MAX_Y);
        for(int j=0; j < MAX_Y; j++) {
            chunk->chunk[i][j] = calloc(CHUNK_SIZE, sizeof(int));
        }
    }

    potion_t potion[60] = {0};
    monster_t monsters[60] = {0};
    door_t doors[60] = {0};
    DoorKey_t keys[60] = {0};
    powerUp_t powerUps[60] = {0};


    file = fopen(path, "r");
    if (file == NULL) {
        logFile("Error opening file");
        return;
    }

    for(int i = CHUNK_SIZE - 1; i >= 0; i--) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            char currChar = fgetc(file);
            switch (currChar) {
                case '#':
                    for (int k = 0; k < WALL_HEIGHT; k++) {
                        chunk->chunk[i][k][j] = rand()% 3 +1;
                    }
                    break;

                case -62:
                    fseek(file, 1, SEEK_CUR);
                    potion[chunk->powerUpCount].pickedUp = 0;
                    potion[chunk->powerUpCount].position = (Vector3) {i, 0, j};
                    chunk->potionCount++;
                    break;

                case 'o':
                    for (int k = DOOR_HEIGHT + 1; k < WALL_HEIGHT; k++) {
                        chunk->chunk[i][k][j] = 1;
                    }

                    doors[chunk->doorCount].opened = 0;
                    doors[chunk->doorCount].position = (Vector3) {i, 0, j};
                    chunk->doorCount++;
                    break;
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
        fpos_t pos;
        fgetpos(file, &pos);
    }

    fgetwc(file);

    char line[30] = {0};
    size_t len = 0;

    chunk->east = NULL;
    chunk->south = NULL;
    chunk->west = NULL;
    chunk->north = NULL;

    while (fgets(line, 30, file) != NULL) {
        logFile(TextFormat("line: %s", line));
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

    for(int i = 0; i < CHUNK_SIZE; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            printf("%d ", chunk->chunk[i][0][j]);
        }
        printf("\n");
    }
    fclose(file);
}


char * concatPath(char * path, char * filename) {
    char * result = malloc(strlen(path) + strlen(filename) + 1);
    strncpy(result, path, strlen(path));
    result[strlen(path)] = '\0';
    strncat(result, filename, strlen(filename) + 1);
    return result;
}

chunk_txt chunkBuffer[MAX_CHUNK_COUNT] = {0};
int chunkCount = 0;

int isChunkInBuffer(int x, int y) {
    for(int i = 0; i < chunkCount; i++) {
        if(chunkBuffer[i].x == x && chunkBuffer[i].y == y) {
            return 1;
        }
    }
    return 0;
}


void createSaveFromLevelFilesR(char * path, char * filename, int x, int y) {
    int currentChunkNo = chunkCount;
    if(isChunkInBuffer(x, y) || filename == NULL) {
        logFile(TextFormat("chunk already loaded: %s", filename));
        return;
    }
    char * fullPath = concatPath(path, filename);
    logFile(TextFormat("Loading chunk: %s", fullPath));
    loadChunkFromTXT(&chunkBuffer[currentChunkNo], fullPath);

    chunkBuffer[currentChunkNo].x = x;
    chunkBuffer[currentChunkNo].y = y;
    chunkCount++;

    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].east, x, y+1);
    logFile(TextFormat("chunk est loaded: %s", fullPath));
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].south, x-1, y);
    logFile(TextFormat("chunk south loaded: %s", fullPath));
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].west, x, y-1);
    logFile(TextFormat("chunk west loaded: %s", fullPath));
    createSaveFromLevelFilesR(path, chunkBuffer[currentChunkNo].north, x+1, y);
    logFile(TextFormat("chunk north loaded: %s", fullPath));

    free(fullPath);
}


void createSaveFromLevelFiles(char * path, char * filename, int fd) {
    index_t index;
    player_t player = BASE_PLAYER;
    Camera camera = BASE_CAMERA;
    player.camera = &camera;
    Vector2 min = {0,0};
    Vector2 max = {0,0};
    chunkCount = 0;
    chunkedMap_t mapContext = {0};
    createSaveFromLevelFilesR(path, filename, 0, 0);

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


    for(int i = 0; i < chunkCount; i++) {
        chunkBuffer[i].x -= min.x;
        chunkBuffer[i].y -= min.y;
    }

    if(fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    index.chunkCount = chunkCount;
    index.chunkCoords = malloc(sizeof(Vector2) * chunkCount);

    index.chunkFilePosition = malloc(sizeof(int*) * chunkCount);


    off_t pos = sizeofIndex(chunkCount) + sizeofPlayerContext() + sizeofMapContext();
    for(int i = 0; i < chunkCount; i++) {
        index.chunkCoords[i] = malloc(sizeof(int) * 2);
        index.chunkCoords[i][0] = chunkBuffer[i].x;
        index.chunkCoords[i][1] = chunkBuffer[i].y;

        index.chunkFilePosition[i] = malloc(sizeof(int) * 2);
        index.chunkFilePosition[i][0] = pos;
        pos += sizeofChunkTXT(chunkBuffer[i]);
        index.chunkFilePosition[i][1] = pos;
    }


    mapContext.centerY = 0;
    mapContext.centerX = 0;
    mapContext.width = 2;
    mapContext.height = 2;
    mapContext.maxX = max.x - min.x + 1;
    mapContext.maxY = max.y - min.y + 1;


    saveMapContext(fd, mapContext);
    savePlayerContext(fd, player);
    writeIndex(fd, index);

    for(int i = 0; i < chunkCount; i++) {
        writeChunkTXT(fd, chunkBuffer[i]);
        freeChunkTXT(chunkBuffer[i]);
    }
    freeIndex(index);
}