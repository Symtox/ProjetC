#include "gameController.h"
#include "physics.h"
#include "renderer.h"
#include "../save/save.h"
#include "../save/fileConverter.h"
#include <fcntl.h>
#include "../utils/utils.h"
#include <string.h>
int controlsToggles[4] = {0, 0, 0, 0};

typedef enum controls {
    FREE_WALK = 0,
    DRAW_CEILING = 1,
    DEBUG = 2,
    DRAW_OVERLAY = 3
}controls_e;

typedef enum fightState {
    NO_FIGHT = -1,
    FIGHT_CHOICE = 0,
    FIGHT_RESULT = 1,
    MONSTER_ATTACK = 2,
}fightState_e;

player_t * player;
chunkedMap_t * map;
fightState_e fightState = NO_FIGHT;
monster_t * monsterInFight = NULL;

int fd = -1;
void pickUpItem();

void initGameController(player_t * playerP, chunkedMap_t * mapPtr, int save) {
    int flags = save ? O_RDWR | O_CREAT: O_RDWR | O_CREAT | O_TRUNC;
    map = mapPtr;
    fd = open("bin/saves/caca", flags, 0666);
    if(!save) {
        createSaveFromLevelFiles("./bin/levels/testLevel/", "niveau1.level", fd);
    }
    loadMapContext(fd, map);
    logFile(TextFormat("mapContext: %d %d %d %d", map->height, map->width, map->maxX, map->maxY));
    loadPlayerFromSave(fd, playerP);
    *map = loadMapFromSave(fd, map->centerX, map->centerY, map->width, map->height, map->maxX, map->maxY);

    player = playerP;
}

void Tick() {
    if(player->inFight) {
        fight();
    } else {
        handlePlayerMovement(player, *map);
        handlePlayerShortcuts();
        loadCurrentMap(fd, map, player->camera->position);
        pickUpItem();
    }
}

void pickUpItem() {
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map);

    if(chunkX == -1 || chunkY == -1) {
        return;
    }

    for(int i = 0; i < map->chunks[chunkX][chunkY].keyCount; i++) {
        if(map->chunks[chunkX][chunkY].keys[i].pickedUp) {
            continue;
        }
        float distanceToItem = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].keys[i].position);
        if(distanceToItem < PICKUP_DETECTION_DISTANCE) {
            map->chunks[chunkX][chunkY].keys[i].pickedUp = 1;
            player->keyCount++;
        }
    }

    for(int i = 0; i < map->chunks[chunkX][chunkY].potionCount; i++) {
        if(map->chunks[chunkX][chunkY].potions[i].pickedUp) {
            continue;
        }
        float distanceToItem = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].potions[i].position);
        if(distanceToItem < PICKUP_DETECTION_DISTANCE) {
            map->chunks[chunkX][chunkY].potions[i].pickedUp = 1;
            player->statistics.health = player->statistics.maxHealth;
        }
    }

    for(int i = 0; i < map->chunks[chunkX][chunkY].powerUpCount; i++) {
        if(map->chunks[chunkX][chunkY].powerUps[i].pickedUp) {
            continue;
        }
        float distanceToItem = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].powerUps[i].position);
        if(distanceToItem < PICKUP_DETECTION_DISTANCE) {
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
    if(player->inFight) {
        return;
    }

    if(IsKeyDown(KEY_F) && !controlsToggles[FREE_WALK]) {
        player->physics.noclip = !player->physics.noclip;
        controlsToggles[FREE_WALK] = 1;
    }


    drawBundle.canOpenDoor = canOpenDoor();
    drawBundle.canOpenFight = canOpenFight();

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
    if(IsKeyPressed(KEY_O) && drawBundle.canOpenDoor == 1) {
        openClosestDoor();
        drawBundle.canOpenDoor = 0;
        player->keyCount--;
    }

    if(IsKeyPressed(KEY_E) && drawBundle.canOpenFight == 1) {
        drawBundle.canOpenFight = 0;
        player->inFight = 1;
        monsterInFight = openClosestFight();
        fightState = FIGHT_CHOICE;
    }
    setDrawBundle(drawBundle);
}


void savePlayer() {
    savePlayerContext(fd, *player);
}

int canOpenDoor() {
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map);
    if(chunkX == -1) {
        return 0;
    }
    for(int i = 0; i < map->chunks[chunkX][chunkY].doorCount; i++) {
        if(map->chunks[chunkX][chunkY].doors[i].opened) {
            continue;
        }
        float distanceToDoor = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].doors[i].position);
        if(distanceToDoor < ACTION_DETECTION_DISTANCE) {
            return player->keyCount > 0 ? 1 : -1;
        }
    }
    return 0;
}

void openClosestDoor() {
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map);
    if(chunkX == -1) {
        return;
    }
    float minDistance = 100000;
    int minIndex = -1;

    for(int i = 0; i < map->chunks[chunkX][chunkY].doorCount; i++) {
        if(map->chunks[chunkX][chunkY].doors[i].opened) {
            continue;
        }
        float distanceToDoor = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].doors[i].position);
        if(distanceToDoor < minDistance) {
            minDistance = distanceToDoor;
            minIndex = i;
        }
    }
    if(minIndex != -1) {
        map->chunks[chunkX][chunkY].doors[minIndex].opened = 1;
    }
}

int canOpenFight() {
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map);
    if(chunkX == -1) {
        return 0;
    }
    for(int i = 0; i < map->chunks[chunkX][chunkY].monsterCount; i++) {

        if(map->chunks[chunkX][chunkY].monsters[i].isDead) {
            continue;
        }
        float distanceToEnemy = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].monsters[i].position);
        if(distanceToEnemy < ACTION_DETECTION_DISTANCE) {
            logFile("Can open fight\n");
            return 1;
        }

    }

    return 0;
}

monster_t * openClosestFight() {
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map);
    if(chunkX == -1) {
        return 0;
    }
    float minDistance = 100000;
    int minIndex = -1;

    for(int i = 0; i < map->chunks[chunkX][chunkY].monsterCount; i++) {
        if(map->chunks[chunkX][chunkY].monsters[i].isDead) {
            continue;
        }
        float distanceToEnemy = distance3D((Vector3){x, player->camera->position.y, z}, map->chunks[chunkX][chunkY].monsters[i].position);
        if(distanceToEnemy < minDistance) {
            minDistance = distanceToEnemy;
            minIndex = i;
        }
    }
    if(minIndex != -1) {
        return &map->chunks[chunkX][chunkY].monsters[minIndex];
    }
    return NULL;
}

void fight() {
    int damageDone;
    int damageTaken;
    fightState_e futureState;
    drawBundle_t drawBundle = getDrawBundle();

    if(monsterInFight == NULL) {
        return;
    }
    damageDone = MAX2(1, player->statistics.damage - monsterInFight->statistics.armor);
    damageTaken = MAX2(1, monsterInFight->statistics.damage - player->statistics.armor);

    logFile(TextFormat("In fight %d\n", (int)fightState));
    switch(fightState) {

        case FIGHT_CHOICE:
            drawBundle.dialog.choiceCount = 2;
            drawBundle.dialog.choices[0] = "Attack";
            drawBundle.dialog.choices[1] = "Run";
            strcpy(drawBundle.dialog.text,"What do you want to do?");
            drawBundle.dialog.keys[0] = KEY_A;
            drawBundle.dialog.keys[1] = KEY_R;

            if(IsKeyPressed(KEY_Q)) {
                monsterInFight->statistics.health -= damageDone;
                fightState = FIGHT_RESULT;
            }

            if(IsKeyPressed(KEY_R)) {
                fightState = NO_FIGHT;
                player->inFight = 0;
                monsterInFight = NULL;
            }
            break;
        case FIGHT_RESULT:

            drawBundle.dialog.choiceCount = 1;
            drawBundle.dialog.choices[0] = "Continue";
            drawBundle.dialog.keys[0] = KEY_SPACE;

            if(monsterInFight->statistics.health <= 0) {
                strcpy(drawBundle.dialog.text, "You killed the monster!");
                monsterInFight->isDead = 1;
                futureState = NO_FIGHT;
            } else {
                sprintf(drawBundle.dialog.text, "You attacked the monster!\n Damage done: %d\n Monster health: %d", damageDone, monsterInFight->statistics.health);
                futureState = MONSTER_ATTACK;
            }

            if(IsKeyPressed(KEY_SPACE)) {
                if(!monsterInFight->isDead) {
                    player->statistics.health -= damageTaken;
                } else {
                    player->statistics.maxHealth++;
                    player->statistics.health++;
                    player->statistics.damage++;
                }
                fightState = futureState;
            }

            break;
        case MONSTER_ATTACK:
            drawBundle.dialog.choiceCount = 1;
            drawBundle.dialog.choices[0] = "Continue";
            sprintf(drawBundle.dialog.text, "The monster attacked you!\nDamage taken: %d\nYour health: %d", damageTaken, player->statistics.health);
            drawBundle.dialog.keys[0] = KEY_SPACE;
            if(IsKeyPressed(KEY_SPACE)) {
                fightState = FIGHT_CHOICE;
            }
            break;
        default:
            player->inFight = 0;
            monsterInFight = NULL;

            break;
    }
    setDrawBundle(drawBundle);
}