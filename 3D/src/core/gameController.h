#ifndef PROJECT_GAMECONTROLLER_H
#define PROJECT_GAMECONTROLLER_H
#include "../../includes/raylib.h"
#include "../board/board.h"
#include "physics.h"
void Tick();
void handlePlayerShortcuts();
void initGameController(player_t *, chunkedMap_t*, int);
void pickUpItem();
int canOpenDoor();
void openClosestDoor();
void savePlayer();
int combatDetection();
int canOpenFight();
monster_t * openClosestFight();
void fight();
#endif
