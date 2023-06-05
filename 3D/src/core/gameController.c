#include "gameController.h"
#include "physics.h"
#include "renderer.h"
#include "../save/save.h"
#include "../save/fileConverter.h"
#include <fcntl.h>
#include "../utils/utils.h"
int controlsToggles[4] = {0, 0, 0, 0};

typedef enum controls {
    FREE_WALK = 0,
    DRAW_CEILING = 1,
    DEBUG = 2,
    DRAW_OVERLAY = 3
}controls_e;

player_t * player;

int fd = -1;
void pickUpItem(chunkedMap_t * map);

void initGameController(player_t * playerP, chunkedMap_t * map, int save) {
    fd = open("bin/saves/caca", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(!save) {
        createSaveFromLevelFiles("./bin/levels/testLevel/", "niveau1.level", fd);
    }
    loadPlayerFromSave(fd, playerP);
    *map = loadMapFromSave(fd, 0, 0, 3, 3);

    player = playerP;
}

void Tick(chunkedMap_t * map) {
    handlePlayerMovement(player, *map);
    handlePlayerShortcuts();
    loadCurrentMap(fd, map, player->camera->position);
    pickUpItem(map);
    //loadCurrentMap(fd, map, player->camera->position);
    //checkForBunuses();
    //loadCurrentChunks();
    //handlerMobMouvements();
    //if((mob = checkForCombat()) != NULL) {
    //    startAFight(mob);
    //}
    //checkForEndOfGame();

}

void pickUpItem(chunkedMap_t * map) {
    int chunkX, chunkY;
    int x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoords(&x, &z, &chunkX, &chunkY, *map);

    if(chunkX == -1 || chunkY == -1) {
        return;
    }

    for(int i = 0; i < map->chunks[chunkX][chunkY].keyCount; i++) {
        if(map->chunks[chunkX][chunkY].keys[i].pickedUp) {
            continue;
        }
        float distanceToItem = distance(x, z, map->chunks[chunkX][chunkY].keys[i].position.x, map->chunks[chunkX][chunkY].keys[i].position.z);
        if(distanceToItem < 1) {
            map->chunks[chunkX][chunkY].keys[i].pickedUp = 1;
            player->keyCount++;
        }
    }

    for(int i = 0; i < map->chunks[chunkX][chunkY].potionCount; i++) {
        if(map->chunks[chunkX][chunkY].potions[i].pickedUp) {
            continue;
        }
        float distanceToItem = distance(x, z, map->chunks[chunkX][chunkY].potions[i].position.x, map->chunks[chunkX][chunkY].potions[i].position.z);
        if(distanceToItem < 1) {
            map->chunks[chunkX][chunkY].potions[i].pickedUp = 1;
            player->statistics.health = player->statistics.maxHealth;
        }
    }

    for(int i = 0; i < map->chunks[chunkX][chunkY].powerUpCount; i++) {
        if(map->chunks[chunkX][chunkY].powerUps[i].pickedUp) {
            continue;
        }
        float distanceToItem = distance(x, z, map->chunks[chunkX][chunkY].powerUps[i].position.x, map->chunks[chunkX][chunkY].powerUps[i].position.z);
        if(distanceToItem < 1) {
            map->chunks[chunkX][chunkY].powerUps[i].pickedUp = 1;
            switch (map->chunks[chunkX][chunkY].powerUps[i].type) {
                case ATTACK:
                    player->statistics.damage += POWER_UP_ATTACK;
                    logFile("Attack power up picked up\n");
                    break;
                case DEFENSE:
                    player->statistics.armor += POWER_UP_DEFENSE;
                    break;
                case MAX_HP:
                    player->statistics.maxHealth += POWER_UP_MAX_HP;
                    break;
            }
        }
    }
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