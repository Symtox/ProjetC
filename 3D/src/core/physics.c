#include "physics.h"
#include "../utils/utils.h"
#include "../../includes/rcamera.h"
#include "../core/renderer.h"
#include <stdlib.h>
#include <stdio.h>
#define APPROX_Y 0.01




/**
 * @brief gestion du mouvement du joueur
 * On somme les vecteurs correspondant aux différentes forces et on vérifie ensuite les colisions
 * @param player
 * @param map
 * @return
 */
int handlePlayerMovement(player_t * player, chunkedMap_t map) {
    drawBundle_t bundle = getDrawBundle();
    Vector3 playerMovement = {0.0f, 0.0f, 0.0f};
    Vector3 playerRotation = getPlayerOrientation(); // Camera rotation

    // Gestion du saut
    playerMovement = Vector3Add(playerMovement, getJumpMovementFromInputs(&player->physics));
    //Gestion du noclip
    playerMovement = Vector3Add(playerMovement, getNoclipMovement(&player->physics));
    //Gestion du mouvement
    playerMovement = Vector3Add(playerMovement, getMovementVectorFromInputs(player->physics.noclip));
    //Gestion de la gravité
    playerMovement = Vector3Add(playerMovement, getFallMovement(player->camera->position, &player->physics, map));
    //Annulation du movement selon x et z si une collision est détectée
    correctMovementWithCollisions(&playerMovement, playerRotation, *player->camera, map);
    bundle.movement = playerMovement;
    bundle.direction = playerRotation;
    setDrawBundle(bundle); // Mise à jour pour l'affichage de débug

    // Mise à jour de la position de la caméra
    updateCameraCustom(player->camera, playerMovement, playerRotation);

    return 0;
}

//Gestion des mouvement verticaux pour le noclip
Vector3 getNoclipMovement(playerPhysics_t * playerPhysics) {
    Vector3 playerMovement = {0.0f, 0.0f, 0.0f};
    if((IsKeyDown(KEY_SPACE) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2))) && playerPhysics->noclip) {
        playerMovement.y += FREE_WALK_MOVEMENT_SPEED;
    }
    if((IsKeyDown(KEY_LEFT_SHIFT) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))) && playerPhysics->noclip) {
        playerMovement.y -= FREE_WALK_MOVEMENT_SPEED;
    }
    return playerMovement;

}

/**
 * @return vitesse en fonction du mode de déplacement
 */
float getMovementSpeed(bool noclip) {
    return noclip ? FREE_WALK_MOVEMENT_SPEED : MOVEMENT_SPEED;
}


/**
 * Calcule le vecteur de mouvement du joueur a partir des inputs
 */
Vector3 getMovementVectorFromInputs(bool noclip) {
    Vector3 playerMovement = {0};
    if(IsGamepadAvailable(0)) {
        float approx = 0.25f;
        playerMovement.x = -GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        playerMovement.z = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        playerMovement.x = fabsf(playerMovement.x) < approx ? 0 : playerMovement.x;
        playerMovement.z = fabsf(playerMovement.z) < approx ? 0 : playerMovement.z;
        return Vector3Scale(playerMovement, getMovementSpeed(noclip));
    } else {
        playerMovement.x = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) - (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN));
        playerMovement.z = (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) - (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT));
        return Vector3Scale(Vector3Normalize(playerMovement), getMovementSpeed(noclip));

    }
}

/**
 * Retourne l'orientation calculé à partir de la souris ou du gamepad
 * @return
 */
Vector3 getPlayerOrientation() {
    if(IsGamepadAvailable(0)) {
        return (Vector3){ GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * PLAYER_SENSITIVITY_GAMEPAD, GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * PLAYER_SENSITIVITY_GAMEPAD, 0.0f};
    }
    return (Vector3){GetMouseDelta().x * PLAYER_SENSITIVITY_MOUSE, GetMouseDelta().y * PLAYER_SENSITIVITY_MOUSE, 0.0f};
}

/**
 * @brief Gestion du saut
 * @param playerPhysics
 * @param playerPhysics
 * @return
 */
Vector3 getJumpMovementFromInputs(playerPhysics_t * playerPhysics) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };

    if(playerPhysics->noclip) {
        return playerMovement;
    }


    handleJump(playerPhysics);
    if(playerPhysics->isJumping && !playerPhysics->isFalling) {
        logFile(TextFormat("Jumping: Jump time: %d\n", playerPhysics->jumpTime));
        if(playerPhysics->jumpTime < JUMP_DURATION) {
            playerMovement.y += getJumpSpeed(*playerPhysics);
            playerPhysics->jumpTime++;
            playerPhysics->jumpingSpeed = getJumpSpeed(*playerPhysics);
        } else {
            playerPhysics->isJumping = 0;
            playerPhysics->jumpTime = 0;
            playerPhysics->jumpingSpeed = JUMP_BASE_SPEED;
        }
    }

    return playerMovement;
}

/**
 * Gestion de la touche de saut
 * @param playerPhysics
 */
void handleJump(playerPhysics_t * playerPhysics) {
    if(IsKeyPressed(KEY_SPACE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
        if(!playerPhysics->isJumping && !playerPhysics->isFalling) {
            playerPhysics->isJumping = 1;
        }
    }

}

/**
 * @brief Gestion de la chute
 * @param playerPosition
 * @param playerPhysics
 * @param map
 * @return
 */
Vector3 getFallMovement(Vector3 playerPosition, playerPhysics_t * playerPhysics, chunkedMap_t map) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };


    float distance = getPlayerDistanceFromGround(playerPosition, map); // calcule de la hauteur de chute


    if(playerPhysics->isJumping || distance == 0 || playerPhysics->noclip) { // Si on saute ou qu'on est au sol, on ne tombe pas
        return playerMovement;
    }

    float fallingSpeed = getFallSpeed(*playerPhysics);

    playerPhysics->isFalling = 1;
    // Cas général
    if(distance > fallingSpeed) {
        playerPhysics->fallTime += 1;
        playerPhysics->fallingSpeed = fallingSpeed;
        playerMovement.y -= fallingSpeed;
    } else { // Si la vitesse est plus grande que la distance du sol on atterit
        playerPhysics->isFalling = 0;
        playerPhysics->fallTime = 0;
        playerPhysics->fallingSpeed = FALL_BASE_SPEED;
        playerMovement.y -= distance;
    }
    return playerMovement;
}

/**
 * Calcule de la vitesse de chute
 * @param player
 * @return
 */
float getFallSpeed(playerPhysics_t player) {
    return MIN2(-1 + pow(3, (float)player.fallTime / 50), MAX_FALL_SPEED); // On augmente la vitesse de chute jusqu'à un maximum (speed = 3^(time/50)
}

/**
 * Calcule de la vitesse de saut
 * @param player
 * @return
 */
float getJumpSpeed(playerPhysics_t player) {
    return player.jumpingSpeed - (player.jumpTime * player.jumpingSpeed / 100) ;
}


/**
 * Calcule de la distance entre le joueur et le sol
 * Un joueur peut théoriquemenbt se trouver sur 4 cases en même temps
 * On cherche donc les coordonnées des 4 cases et on prend la plus petite distance
 * ON finit par soustraire la taille du joueur (hauteur de la caméra) pour avoir la distance entre le sol et les pied du joueur
 * @param playerPosition
 * @param map
 * @return
 */
float getPlayerDistanceFromGround(Vector3 playerPosition, chunkedMap_t map) {
    int caseAX = (int)playerPosition.x;
    int caseAZ = (int)playerPosition.z;
    int caseBX;
    int caseBZ;
    int multipleX = 0;
    int multipleZ = 0;
    float minDistance;

    if((int)playerPosition.x != (int)(playerPosition.x + PLAYER_WIDTH)) {
        multipleX = 1;
        caseBX = (int)(playerPosition.x + PLAYER_WIDTH);
    }
    else if((int)playerPosition.x != (int)(playerPosition.x - PLAYER_WIDTH)) {
        multipleX = 1;
        caseBX = (int)(playerPosition.x - PLAYER_WIDTH);
    }

    if((int)playerPosition.z != (int)(playerPosition.z + PLAYER_WIDTH)) {
        multipleZ = 1;
        caseBZ = (int)(playerPosition.z + PLAYER_WIDTH);
    }
    else if((int)playerPosition.z != (int)(playerPosition.z - PLAYER_WIDTH)) {
        multipleZ = 1;
        caseBZ = (int)(playerPosition.z - PLAYER_WIDTH);
    }

    if(multipleX && multipleZ) {
        minDistance = MIN4(
                getTileDistanceFromGround(caseAX, playerPosition.y, caseAZ, map),
                getTileDistanceFromGround(caseAX, playerPosition.y, caseBZ, map),
                getTileDistanceFromGround(caseBX, playerPosition.y, caseAZ, map),
                getTileDistanceFromGround(caseBX, playerPosition.y, caseBZ, map)
        );
    }

    else if (multipleX) {
        minDistance = MIN2(
                getTileDistanceFromGround(caseAX, playerPosition.y, caseAZ, map),
                getTileDistanceFromGround(caseBX, playerPosition.y, caseAZ, map)
        );
    }

    else if (multipleZ) {
        minDistance = MIN2(
                getTileDistanceFromGround(caseAX, playerPosition.y, caseAZ, map),
                getTileDistanceFromGround(caseAX, playerPosition.y, caseBZ, map)
        );
    } else {
        minDistance = getTileDistanceFromGround(playerPosition.x, playerPosition.y, playerPosition.z, map);
    }

    return minDistance - PLAYER_HEIGHT;
}

/**
 * Annulation du mouvement selon x et z si il y a collision
 * Le joueur peut là aussi etre sur 4 cases en même temps, on cherche donc les 4 cases + les 4 cases plus hautes car la taille du joueur est de 2 (environ)
 * @param movement
 * @param playerRotation
 * @param camera
 * @param playerPhysics
 * @param map
 */
void correctMovementWithCollisions(Vector3 * movement, Vector3 playerRotation, Camera camera, chunkedMap_t map) {

    int caseAX;
    int caseAZ;
    int caseBX;
    int caseBZ;
    int multipleX = 0;
    int multipleZ = 0;
    Camera cameraAfterMove = camera;
    updateCameraCustom(&cameraAfterMove, *movement, playerRotation);
    caseAX = (int)cameraAfterMove.position.x;
    caseAZ = (int)cameraAfterMove.position.z;

    // On cherche les 4 cases autour du joueur
    if((int)cameraAfterMove.position.x != (int)(cameraAfterMove.position.x + PLAYER_WIDTH)) {
        multipleX = 1;
        caseBX = (int)(cameraAfterMove.position.x + PLAYER_WIDTH);
    }
    else if((int)cameraAfterMove.position.x != (int)(cameraAfterMove.position.x - PLAYER_WIDTH)) {
        multipleX = 1;
        caseBX = (int)(cameraAfterMove.position.x - PLAYER_WIDTH);
    }

    if((int)(cameraAfterMove.position.z) != (int)(cameraAfterMove.position.z + PLAYER_WIDTH)) {
        multipleZ = 1;
        caseBZ = (int)(cameraAfterMove.position.z + PLAYER_WIDTH);
    }
    else if((int)(cameraAfterMove.position.z) != (int)(cameraAfterMove.position.z - PLAYER_WIDTH)) {
        multipleZ = 1;
        caseBZ = (int)(cameraAfterMove.position.z - PLAYER_WIDTH);
    }

    // On vérifie si les 4 cases sont libres + les 4 cases plus hautes
    int isMoveOk = 1;
    for(int i=0; i < PLAYER_HEIGHT; i++) {
        if(multipleX && multipleZ) {
            isMoveOk = isMoveOk && isTileFree(caseBX, (int)(cameraAfterMove.position.y - PLAYER_HEIGHT + i + 1), caseBZ, map);
        }
        if (multipleX) {
            isMoveOk = isMoveOk && isTileFree(caseBX, (int)(cameraAfterMove.position.y - PLAYER_HEIGHT + i + 1), caseAZ, map);
        }
        if (multipleZ) {
            isMoveOk = isMoveOk && isTileFree(caseAX, (int)(cameraAfterMove.position.y - PLAYER_HEIGHT + i + 1), caseBZ, map);
        }
        isMoveOk = isMoveOk && isTileFree(caseAX, (int)(cameraAfterMove.position.y - PLAYER_HEIGHT + i + 1), caseAZ, map);
    }

    if(isMoveOk) {
        return;
    }

    //Annulation du mouvement en x et z (si on percutte un mur on veut pouvoir tomber
    //TODO calcul + précis de la distance à laquelle on peut se déplacer + gestion des angles + collision avec le plafond


    movement->x = 0;
    movement->z = 0;
}


/**
 * Met à jour la caméra en fonction du mouvement et de la rotation du joueur
 * @param camera
 * @param movement
 * @param rotation
 */
void updateCameraCustom(Camera * camera, Vector3 movement, Vector3 rotation) {

    bool lockView = true;
    bool rotateAroundTarget = false;
    bool rotateUp = false;
    bool moveInWorldPlane = true;

    // Camera rotation
    CameraPitch(camera, -rotation.y*DEG2RAD, lockView, rotateAroundTarget, rotateUp);
    CameraYaw(camera, -rotation.x*DEG2RAD, rotateAroundTarget);

    // Camera movement
    CameraMoveForward(camera, movement.x, moveInWorldPlane);
    CameraMoveRight(camera, movement.z, moveInWorldPlane);
    CameraMoveUp(camera, movement.y);

}

/**
 * Calcul la distance entre une case et le sol
 * @param x
 * @param y
 * @param z
 * @param map
 * @return
 */
float getTileDistanceFromGround(int x, float y, int z, chunkedMap_t map) {
    int chunkIndexX, chunkIndexY;
    if(x < 0 || z < 0 || x >= CHUNK_SIZE * map.maxY || z >= CHUNK_SIZE * map.maxX || y < 0 || y >= MAX_Y) {
        return 10;
    }
    toChunkCoords(&x, &z, &chunkIndexX, &chunkIndexY, map);
    if(chunkIndexX == -1 || chunkIndexY == -1) { // Si le joueur est en dehors de la map
        return 10;
    }


    for(int i=y - PLAYER_HEIGHT; i >= 0; i--) {
        if(map.chunks[chunkIndexX][chunkIndexY].chunk[x][i][z] != 0) {
            return y - i;
        }
    }


    return y;
}

/**
 * Verifie qu'une case est libre
 * @param x
 * @param y
 * @param z
 * @param map
 * @return
 */
int isTileFree(int x, int y, int z, chunkedMap_t map) {
    int chunkIndexX, chunkIndexY;
    bool isDoorFree = true;
    bool isMonsterFree = true;

    if(x < 0 || z < 0 || x >= CHUNK_SIZE * map.maxX || z >= CHUNK_SIZE * map.maxY || y < 0 || y >= MAX_Y) { // Case en dehors de la carte (vide)
        return 1;
    }
    toChunkCoords(&x, &z, &chunkIndexX, &chunkIndexY, map);
    if(chunkIndexX == -1 || chunkIndexY == -1) {
        return 1;
    }

    //Verification des portes
    for(int i = 0; i < map.chunks[chunkIndexX][chunkIndexY].doorCount; i++) {
        if(map.chunks[chunkIndexX][chunkIndexY].doors[i].position.x == x
            && (
                    map.chunks[chunkIndexX][chunkIndexY].doors[i].position.y == y
                    || map.chunks[chunkIndexX][chunkIndexY].doors[i].position.y + 1 == y
              )
            && map.chunks[chunkIndexX][chunkIndexY].doors[i].position.z == z
            && !map.chunks[chunkIndexX][chunkIndexY].doors[i].opened) {
                isDoorFree = false;
        }
    }

    //Verification des monstres
    for(int i = 0; i < map.chunks[chunkIndexX][chunkIndexY].monsterCount; i++) {
        if(map.chunks[chunkIndexX][chunkIndexY].monsters[i].position.x == x
            && (
                    map.chunks[chunkIndexX][chunkIndexY].monsters[i].position.y+3 == y
                    || map.chunks[chunkIndexX][chunkIndexY].monsters[i].position.y + 1 == y
                   || map.chunks[chunkIndexX][chunkIndexY].monsters[i].position.y + 2 == y
              )
            && map.chunks[chunkIndexX][chunkIndexY].monsters[i].position.z == z) {

                isMonsterFree = map.chunks[chunkIndexX][chunkIndexY].monsters[i].isDead;
        }
    }

    // Si il n'y a pas de bloc, pas de monstre et pas de porte
    return map.chunks[chunkIndexX][chunkIndexY].chunk[x][y][z] == 0
        && isDoorFree
        && isMonsterFree;
}

/**
 * Converti les coordonnées de la map globale (utilisé pour l'affichage) en coordonnées de chunk
 * @param x
 * @param z
 * @param chunkX
 * @param chunkY
 * @param map
 */
void toChunkCoords(int * x, int * z, int * chunkX, int * chunkY, chunkedMap_t map) {
    int playerChunkX = (int)(*x / CHUNK_SIZE);
    int playerChunkY = (int)(*z / CHUNK_SIZE);
    *chunkX = -1;
    *chunkY = -1;
    *x = (int)(*x - playerChunkX * CHUNK_SIZE);
    *z = (int)(*z - playerChunkY * CHUNK_SIZE);


    for(int i=0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            if(map.chunks[i][j].x != -1 && map.chunks[i][j].y != -1 && map.chunks[i][j].x == playerChunkX && map.chunks[i][j].y == playerChunkY) {
                *chunkX = i;
                *chunkY = j;
            }
        }
    }
}

/**
 * Pareil qu'au dessus mais pour les float
 * @param x
 * @param z
 * @param chunkX
 * @param chunkY
 * @param map
 */
void toChunkCoordsF(float * x, float * z, int * chunkX, int * chunkY, chunkedMap_t map) {
    int playerChunkX = (int)((int)*x / CHUNK_SIZE);
    int playerChunkY = (int)((int)*z / CHUNK_SIZE);
    *chunkX = -1;
    *chunkY = -1;
    *x = *x - playerChunkX * CHUNK_SIZE;
    *z = *z - playerChunkY * CHUNK_SIZE;


    for(int i=0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            if(map.chunks[i][j].x != -1 && map.chunks[i][j].y != -1 && map.chunks[i][j].x == playerChunkX && map.chunks[i][j].y == playerChunkY) {
                *chunkX = i;
                *chunkY = j;
            }
        }
    }
}





//Bullets physics


