#include "gameController.h"
#include "../utils/const.h"
#include "physics.h"
#include "renderer.h"
#include "../entities.h"
#include "../save/save.h"

int controlsToggles[4] = {0, 0, 0, 0};

typedef enum controls {
    FREE_WALK = 0,
    DRAW_CEILING = 1,
    DEBUG = 2,
    DRAW_OVERLAY = 3
}controls_e;

player_t * player;

void initGameController(player_t * playerP, chunkedMap_t * map, int loadMapFromSave) {
    if(!loadMapFromSave) {
        //createSaveFromLevelFiles("bin/saves/board.bin", "bin/levels/board/board.csv");
    }
    *map = loadMap("bin/levels/board/board.csv", 0, 0, 3, 3, 0);
    player = playerP;
}

void Tick(chunkedMap_t * map) {
    handlePlayerMovement(player, *map);
    handlePlayerShortcuts();
    loadCurrentMap(map, player->camera->position);
    //checkForBunuses();
    //loadCurrentChunks();
    //handlerMobMouvements();
    //if((mob = checkForCombat()) != NULL) {
    //    startAFight(mob);
    //}
    //checkForEndOfGame();

}


void handlePlayerShortcuts() {
    drawBundle_t drawBundle = getDrawBundle();
    if(IsKeyDown(KEY_F) && !controlsToggles[FREE_WALK]) {
        player->physics.noclip = !player->physics.noclip;
        controlsToggles[FREE_WALK] = 1;
    }
    if(IsKeyUp(KEY_F) && controlsToggles[FREE_WALK]) {
        controlsToggles[FREE_WALK] = 0;
    }

    if(IsKeyDown(KEY_C) && !controlsToggles[DRAW_CEILING]) {
        drawBundle.drawCeiling = !drawBundle.drawCeiling;
        controlsToggles[DRAW_CEILING] = 1;
    }
    if(IsKeyUp(KEY_C) && controlsToggles[DRAW_CEILING]) {
        controlsToggles[DRAW_CEILING] = 0;
    }

    if(IsKeyDown(KEY_F3) && !controlsToggles[DEBUG]) {
        drawBundle.drawDebug = !drawBundle.drawDebug;
        controlsToggles[DEBUG] = 1;
    }
    if(IsKeyUp(KEY_F3) && controlsToggles[DEBUG]) {
        controlsToggles[DEBUG] = 0;
    }

    if(IsKeyDown(KEY_F1) && !controlsToggles[DRAW_OVERLAY]) {
        drawBundle.drawOverlay = !drawBundle.drawOverlay;
        controlsToggles[DRAW_OVERLAY] = 1;
    }
    if(IsKeyUp(KEY_F1) && controlsToggles[DRAW_OVERLAY]) {
        controlsToggles[DRAW_OVERLAY] = 0;
    }
    setDrawBundle(drawBundle);
}