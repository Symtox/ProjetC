/*Déplacement du joueur sur une clé*/
void moveOnKey(player_t * player, int x, int y);

/*Déplacement du joueur sur une porte*/
void moveOnDoor(player_t * player, int x, int y);

/*Déplacement du joueur sur une potion*/
void moveOnPotion(player_t * player, int x, int y);

/*Déplacement du joueur sur un powerup*/
void moveOnPowerUp(player_t * player, int x, int y, int power);

/*Gestion des déplacements du joueur*/
void movements(map_t * map, player_t * player, tabMonsters_t ** tabMonsters, int addX, int addY);
