#ifndef PROJECT_GAMECONTROLLER_H
#define PROJECT_GAMECONTROLLER_H
#include "../../includes/raylib.h"
#include "../board/board.h"
#include "physics.h"
void Tick(chunkedMap_t *);
void handlePlayerShortcuts();
void initGameController(player_t *, chunkedMap_t*, int);
void pickUpItem(chunkedMap_t *);
void savePlayer();
#endif
