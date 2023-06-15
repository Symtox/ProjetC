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
#include "../menu/mainMenu.h"
int controlsToggles[4] = {0, 0, 0, 0};

bullet_t * bullets = NULL;
int bulletCount = 0;

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
int isGamePaused = 0;
int isWin = 0;
int godMode = 0;

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
    if(!isGamePaused && !isWin) {
        if(player->statistics.health <= 0) {
            drawBundle_t bundle = getDrawBundle();
            bundle.canOpenFight = 0;
            bundle.canOpenDoor = 0;
            bundle.player->inFight = 0;
            setDrawBundle(bundle);
        } else if(player->inFight) {
            fight();
        } else {
            handlePlayerMovement(player, *map);
            handlePlayerShortcuts();
            loadCurrentMap(fd, map, player->camera->position);
            pickUpItem();
            shoot();
            endGame();
        }
    }
    handlePause();
    if(isGamePaused) {
        handlePauseMenuAction();
    }
}

void handlePause() {
    drawBundle_t drawBundle = getDrawBundle();
    if(IsKeyPressed(KEY_ESCAPE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))) {
        isGamePaused = !isGamePaused;
        drawBundle.paused = isGamePaused;
        if(isGamePaused) {
            EnableCursor();
            SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
        } else {
            DisableCursor();
        }
    }
    setDrawBundle(drawBundle);

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

    if(IsKeyPressed(KEY_P) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))) {
        player->statistics.health = 1000;
        player->statistics.maxHealth = 1000;
        player->statistics.damage = 1000;
        player->statistics.armor = 1000;
        player->keyCount = 99;
        godMode = 1;
        drawBundle.godMode = 1;
    }

    if((IsKeyDown(KEY_F) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))) && !controlsToggles[FREE_WALK]) {
        player->physics.noclip = !player->physics.noclip;
        controlsToggles[FREE_WALK] = 1;
    }


    drawBundle.canOpenDoor = canOpenDoor();
    drawBundle.canOpenFight = canOpenFight();

    if((IsKeyUp(KEY_F) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))) && controlsToggles[FREE_WALK]) {
        controlsToggles[FREE_WALK] = 0;
    }

    if((IsKeyDown(KEY_C) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))) && !controlsToggles[DRAW_CEILING]) {
        drawBundle.drawCeiling = !drawBundle.drawCeiling;
        controlsToggles[DRAW_CEILING] = 1;
    }
    if((IsKeyUp(KEY_C) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))) && controlsToggles[DRAW_CEILING]) {
        controlsToggles[DRAW_CEILING] = 0;
    }

    if((IsKeyDown(KEY_F3) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))) && !controlsToggles[DEBUG]) {
        drawBundle.drawDebug = !drawBundle.drawDebug;
        controlsToggles[DEBUG] = 1;
    }
    if((IsKeyUp(KEY_F3) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))) && controlsToggles[DEBUG]) {
        controlsToggles[DEBUG] = 0;
    }

    if((IsKeyDown(KEY_F1) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))) && !controlsToggles[DRAW_OVERLAY]) {
        drawBundle.drawOverlay = !drawBundle.drawOverlay;
        controlsToggles[DRAW_OVERLAY] = 1;
    }
    if((IsKeyUp(KEY_F1) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))) && controlsToggles[DRAW_OVERLAY]) {
        controlsToggles[DRAW_OVERLAY] = 0;
    }
    if((IsKeyPressed(KEY_O) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))) && drawBundle.canOpenDoor == 1) {
        openClosestDoor();
        drawBundle.canOpenDoor = 0;
        player->keyCount--;
    }

    // Entrée en combat
    if((IsKeyPressed(KEY_E) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))) && drawBundle.canOpenFight == 1) {
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



void shoot() {
    drawBundle_t drawBundle = getDrawBundle();
    if(!godMode) {
        return;
    }

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bullet_t bullet;
        Ray ray = GetMouseRay(GetMousePosition(), *player->camera);
        bullet.position = ray.position;
        Vector3 direction = Vector3Normalize(ray.direction);
        bullet.direction.x = direction.z;
        bullet.direction.y = Vector3Normalize(player->camera->target).y;
        bullet.direction.z = -direction.x;
        bullet.timeToLive = 50;
        addBullet(bullet);
        drawBundle.bulletCount = bulletCount;
        drawBundle.bullets = bullets;
    }
    updateBullets();
    setDrawBundle(drawBundle);

}

void addBullet(bullet_t target) {
    for(int i = 0; i < bulletCount; i++) {
        if(bullets[i].timeToLive <= 0) {
            bullets[i].position = target.position;
            bullets[i].direction = target.direction;
            bullets[i].timeToLive = target.timeToLive;
            logFile(TextFormat("remplacement d'une balle %d", bulletCount));
            return;
        }
    }
    logFile(TextFormat("Ajout d'une balle %d", bulletCount));
    bullets = bulletCount == 0 ? malloc(sizeof(bullet_t)) : realloc(bullets, sizeof(bullet_t) * (bulletCount + 1));
    bullets[bulletCount].position = target.position;
    bullets[bulletCount].direction = target.direction;
    bullets[bulletCount].timeToLive = target.timeToLive;
    bulletCount++;


}

void updateBullets() {
    for(int i = 0; i < bulletCount; i++) {
        float bulletX = bullets[i].position.x;
        float bulletZ = bullets[i].position.z;
        float bulletY = bullets[i].position.y + 0.7f;
        int chunkX;
        int chunkY;
        if(bullets[i].timeToLive <= 0) {
            bullets[i].position.x = -1;
            bullets[i].position.y = -1;
            bullets[i].position.z = -1;
            bullets[i].direction.x = 0;
            bullets[i].direction.y = 0;
            bullets[i].direction.z = 0;
            bullets[i].timeToLive = 0;
            continue;
        }
        bullets[i].position.x += bullets[i].direction.x * BULLET_SPEED;
        bullets[i].position.z += bullets[i].direction.z * BULLET_SPEED;
        bullets[i].position.y += bullets[i].direction.y * BULLET_SPEED;
        bullets[i].timeToLive--;

        toChunkCoordsF(&bulletX, &bulletZ, &chunkX, &chunkY, *map);
        if(chunkX == -1 || chunkY == -1 || bulletY < 0 || bulletY >= MAX_Y || bulletX < 0 || bulletX >= CHUNK_SIZE || bulletZ < 0 || bulletZ >= CHUNK_SIZE) {
            continue;
        }
        map->chunks[chunkX][chunkY].chunk[(int)bulletX][(int)bulletY][(int)bulletZ] = 0;
        for(int j = 0; j < map->chunks[chunkX][chunkY].monsterCount; j++) {
            if((int)map->chunks[chunkX][chunkY].monsters[j].position.x == (int)bulletX && (int)map->chunks[chunkX][chunkY].monsters[j].position.z == (int)bulletZ) {
                map->chunks[chunkX][chunkY].monsters[j].isDead = 1;
                bullets[i].timeToLive = 0;
            }
        }

        for(int j = 0; j < map->chunks[chunkX][chunkY].doorCount; j++) {
            if((int)map->chunks[chunkX][chunkY].doors[j].position.x == (int)bulletX && (int)map->chunks[chunkX][chunkY].doors[j].position.z == (int)bulletZ) {
                map->chunks[chunkX][chunkY].doors[j].opened = 1;
                bullets[i].timeToLive = 0;
            }
        }
    }


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
            drawBundle.dialog.keys[0] = 'A';
            drawBundle.dialog.keys[1] = IsGamepadAvailable(0) ? 'B' : 'R';
            if(IsKeyPressed(KEY_Q) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
                monsterInFight->statistics.health -= damageDone;
                fightState = FIGHT_RESULT;
            }

            if(IsKeyPressed(KEY_R) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))) {
                fightState = NO_FIGHT;
                player->inFight = 0;
                monsterInFight = NULL;
            }
            break;
        //Si le joueur a choisi d'attaquer
        case FIGHT_RESULT:

            drawBundle.dialog.choiceCount = 1;
            drawBundle.dialog.choices[0] = "Continue";
            drawBundle.dialog.keys[0] = IsGamepadAvailable(0) ? 'A' : 'E';

            if(monsterInFight->statistics.health <= 0) {
                strcpy(drawBundle.dialog.text, "You killed the monster!");
                monsterInFight->isDead = 1;
                futureState = NO_FIGHT;
            } else {
                sprintf(drawBundle.dialog.text, "You attacked the monster!\n Damage done: %d\n Monster health: %d", damageDone, monsterInFight->statistics.health);
                futureState = MONSTER_ATTACK;
            }

            if(IsKeyPressed(KEY_E) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
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
            drawBundle.dialog.keys[0] = IsGamepadAvailable(0) ? 'A' : 'E';
            if(IsKeyPressed(KEY_E) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
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
    if(bulletCount > 0) {
        free(bullets);
    }
    bulletCount = 0;
    destroyRenderer();
    savePlayerContext(fd, *player);
    saveMap(map, fd);
    freeMap(map);
    isLoaded = 0;
}

void handlePauseMenuAction() {
    drawBundle_t drawBundle = getDrawBundle();
    Vector2 mousePoint;
    mousePoint = GetMousePosition();

    if(CheckCollisionPointRec(mousePoint, getPauseMenuResumeButtonBounds())) {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isGamePaused = false;
            drawBundle.paused = false;
            DisableCursor();
        }
    }
    if(CheckCollisionPointRec(mousePoint, getPauseMenuQuitButtonBounds())) {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            saveAndQuit();
            DisableCursor();
            isGamePaused = false;
            drawBundle.paused = false;
            drawBundle.win = 0;
            isWin = 0;
            godMode = 0;
            monsterInFight = NULL;
            player->inFight = 0;

            setCurrentScene(MAIN_MENU_VIEW);
        }
    }
    setDrawBundle(drawBundle);

}

/**
 * Check si le joueur est suffisament proche de la case de fin
 */
void endGame() {
    drawBundle_t drawBundle = getDrawBundle();
    int chunkX, chunkY;
    float x = player->camera->position.x, z = player->camera->position.z;
    toChunkCoordsF(&x, &z, &chunkX, &chunkY, *map);
    if(chunkX == -1 || map->chunks[chunkX][chunkY].endGameX == -1 || map->chunks[chunkX][chunkY].endGameY == -1) {
        return;
    }
    float distanceToFinish = distance3D((Vector3){x, player->camera->position.y, z}, (Vector3){map->chunks[chunkX][chunkY].endGameX, 0, map->chunks[chunkX][chunkY].endGameY});
    if(distanceToFinish < 2) {
        drawBundle.win = 1;
        isWin = 1;
    }
    setDrawBundle(drawBundle);
}