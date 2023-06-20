#include "../entities/entities.h"

/*Regarde si la classe de monstre a déjà été lu*/
int isMonsterClassFound(char * monsters, char monster, int nbClassFound);

/*Lecture du fichier de niveau et enregistre les cases dans la structure map*/
map_t readMap(char * path);

/*Fonction substring*/
char * substr(char *src,int pos);

/*Initialisation du niveau*/
map_t initMap(char * fileName, tabMonsters_t ** tabMonsters, int isFirstInit);