#include "../entities/entities.h"

int isMonsterClassFound(char * monsters, char monster, int nbClassFound);

map_t readTXT(char * path);

char * substr(char *src, int pos);

map_t initMap(char * fileName, tabMonsters_t ** tabMonsters);