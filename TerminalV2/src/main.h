/*Affichage des crédits*/
void credits();

/*Fonction de coloration de la console (t = font-color, f = background-color)*/
void color(int t,int f);

/*Affichage en couleur selon les items*/
void printItemColor(char item);

/*Affichage plateau*/
void printMap(map_t * map, player_t * player);

/*Initialisation du jeu*/
void play(map_t * map, player_t * player, tabMonsters_t ** tabMonsters, tabMaps_t * tabMaps);

/*Libère la mémoire à la fin de la partie*/
void endGame(tabMaps_t * tabMaps, tabMonsters_t ** tabMonsters);

/*Lance une nouvelle partie*/
void startNewGame();

/*Menu affiché lorsqu'on lance le jeu*/
void startMenu();