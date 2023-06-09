#include "gameController.h"
#include "physics.h"
#include "renderer.h"
#include "../save/save.h"
#include "../save/fileConverter.h"
#include <fcntl.h>
#include "../utils/utils.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
int isLoaded = 0;

int fd = -1;
void pickUpItem();

/**
 * Initialise la sauvegarde si besoin, sinon charge la sauvegarde
 * @param playerP: pointeur vers le joueur
 * @param mapPtr: pointeur vers la map
 * @param save: 1 si on veut jouer depuis une sauvegarde existante, 0 sinon
 */
void initGameController(player_t * playerP, chunkedMap_t * mapPtr, char * savePath) {
    if(isLoaded) {
        return;
    }
    DisableCursor();
    srand(time(NULL));
    printf("savePath: %s\n", savePath);
    if(savePath == NULL) {
        char * fullPath = malloc(sizeof(char) * 100);
        char * saveName = malloc(sizeof(char) * 100);
        sprintf(saveName, "save%d.bin", rand() % 1000);
        strcpy(fullPath, "./bin/saves/");
        strcat(fullPath, saveName);
        fd = open(fullPath, O_RDWR | O_CREAT | O_TRUNC, 0666);
        createSaveFromLevelFiles("./bin/levels/testLevel/", "niveau1.level", fd);
        free(fullPath);
        free(saveName);
        free(savePath);
    } else {
        char * fullPath = malloc(sizeof(char) * 100);
        strcpy(fullPath, "./bin/saves/");
        strcat(fullPath, savePath);
        fd = open(fullPath, O_RDWR, 0666);
    }
    map = mapPtr;
    loadMapContext(fd, map);
    loadPlayerFromSave(fd, playerP);
    *map = loadMapFromSave(fd, map->centerX, map->centerY, map->width, map->height, map->maxX, map->maxY);

    player = playerP;
    isLoaded = 1;
}

/**
 * Gère un tick de jeu
 * Si joueur en combat bloque les actions
 * Sinon Appelle les fonctions de gestion du joueur et de la map
 */
void Tick() {
    if(player->statistics.health <= 0) {
        drawBundle_t bundle = getDrawBundle();
        bundle.canOpenFight = 0;
        bundle.canOpenDoor = 0;
        bundle.player->inFight = 0;
        setDrawBundle(bundle);
        return;
    } else if(player->inFight) {
        fight();
    } else {
        handlePlayerMovement(player, *map);
        handlePlayerShortcuts();
        loadCurrentMap(fd, map, player->camera->position);
        pickUpItem();
    }
}

/**
 * Gere la récupération d'item
 */
void pickUpItem() {
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map); // Conversion des coordonnées du joueur en coordonnées de chunk

    if(chunkX == -1 || chunkY == -1) {
        return;
    }
    //On parcourt les items du chunk et on regarde si le joueur est à proximité
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
                    player->statistics.damage += 1;
                    break;
                case DEFENSE:
                    player->statistics.armor += 1;
                    break;
                case MAX_HP:
                    player->statistics.maxHealth += 1;
                    break;
            }
        }
    }
}


/**
 * Gère les actions du joueur (excepté le mouvement).
 * LEs toggles sont utilisés pour éviter que l'action ne se répète à chaque tick
 */
void handlePlayerShortcuts() {
    drawBundle_t drawBundle = getDrawBundle(); // On recupere les informations de dessin afin de les mettre à jour
    if(player->inFight) {
        return;
    }

    if(IsKeyPressed(KEY_P)) {
        player->statistics.health = 1000;
        player->statistics.maxHealth = 1000;
        player->statistics.damage = 1000;
        player->statistics.armor = 1000;
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

    // Entrée en combat
    if(IsKeyPressed(KEY_E) && drawBundle.canOpenFight == 1) {
        drawBundle.canOpenFight = 0;
        player->inFight = 1;
        monsterInFight = openClosestFight();
        fightState = FIGHT_CHOICE;
    }
    setDrawBundle(drawBundle);
}

//Sauvegarde de toutes les infos de la partie
void save() {
    savePlayerContext(fd, *player);
    saveMapContext(fd, *map);

}

/**
 * vérifie si le joueur est à proximité d'un porte + si il a une clé
 * @return 0 = pas de porte, 1 = peut ouvrir, -1 = pas de clé
 */
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

/**
 * Ouvre la porte la plus proche et consomme une clef
 */
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

/**
 * Verifie si un monstre est  à proximité du joueur
 */
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
            return 1;
        }

    }

    return 0;
}

/**
 * Ouvre le combat avec le monstre le plus proche
 */
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


/**
 * Gestion du combat
 * TODO ajouter les textures des touches
 * TODO map les touches de manettes
 */
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

    //Selon l'état du combat on défini les actions possibles
    switch(fightState) {
        //Si le combat n'a pas commencé / le tour du monstre est terminé
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
        //Si le joueur a choisi d'attaquer
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
        // Attaque du monstre
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
    setDrawBundle(drawBundle); //UPdate des options d'affichage
}


/**
 * Si le jeu est chargé, on sauevgarde et on free les ressources
 * TODO: free index
 */
void saveAndQuit() {
    if(!isLoaded) {
        return;
    }
    savePlayerContext(fd, *player);
    saveMap(map, fd);
    freeMap(map);

}