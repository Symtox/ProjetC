#include "gameController.h"
#include "../utils/const.h"
#include "physics.h"
void Tick(Camera * camera, playerPhysics_t * playerPhysics, map_t * map) {
    //mob_t combatMob;
    handlePlayerMovement(camera, playerPhysics, *map);
    loadCurrentMap(map, camera->position);
    //loadCurrentChunks();
    //handlerMobMouvements();
    //if((mob = checkForCombat()) != NULL) {
    //    startAFight(mob);
    //}
    //checkForBunuses();
    //checkForEndOfGame();

}