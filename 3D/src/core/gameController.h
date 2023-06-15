#ifndef PROJECT_GAMECONTROLLER_H
#define PROJECT_GAMECONTROLLER_H
#include "../../includes/raylib.h"
#include "../board/board.h"
#include "physics.h"
#include "../utils/utils.h"
void Tick();
void handlePlayerShortcuts();
void initGameController(player_t *, chunkedMap_t*, char *);
void pickUpItem();
int canOpenDoor();
void openClosestDoor();
void savePlayer();
int combatDetection();
int canOpenFight();
monster_t * openClosestFight();
void fight();
void saveAndQuit();
void handlePauseMenuAction();
void handlePause();
void endGame();
void shoot();
void addBullet(bullet_t target);
void updateBullets();
#endif
