/*Création plateau*/
plateau * createPlateau()

/*Affichage plateau*/
void printPlateau(plateau * game, player_t player)

/*Déplacement du joueur sur une clé*/
player_t moveOnKey(player_t player, int x, int y)

/*Déplacement du joueur sur une porte*/
player_t moveOnDoor(player_t player, int x, int y)

/*Déplacement du joueur sur une potion*/
player_t moveOnPotion(player_t player, int x, int y)

/*Déplacement du joueur sur un powerup*/
player_t moveOnPowerUp(player_t player, int x, int y, int power)

/*Gestion des déplacements du joueur*/
player_t movements(plateau * game, player_t player, int addX, int addY)

/*Initialisation du jeu*/
void play(plateau * game, player_t player)
