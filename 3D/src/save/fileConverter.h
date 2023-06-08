#ifndef PROJECT_FILE_CONVERTER_H
#define PROJECT_FILE_CONVERTER_H
#include <stdio.h>
#include "../board/board.h"
void createSaveFromLevelFiles(char * path, char * filename, int);
void createSaveFromLevelFilesR(char * path, char * filename, int x, int y);
char * concatPath(char * path, char * filename);
void loadChunkFromTXT(chunk_txt * chunk, char* filename);
char * substr(char *src, int pos);
ssize_t getLineFromText(char **lineptr, FILE *stream);


#endif