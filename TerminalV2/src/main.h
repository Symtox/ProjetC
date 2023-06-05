/*Menu affiché lorsqu'on lance le jeu*/
void startMenu();

/*Affichage plateau*/
void printMap(map_t * map, player_t * player);

/*Initialisation du jeu*/
void play(map_t * map, player_t * player, tabMonsters_t ** tabMonsters, tabMaps_t * tabMaps);
