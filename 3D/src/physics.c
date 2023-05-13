#include "physics.h"
#include "../includes/raylib.h"
#include "./Const.h"
#include <stdlib.h>
#include "utils.h"





int handlePlayerMovement(Camera *camera, playerPhysics_t * playerPhysics, int ** map) {
    float playerZoom = GetMouseWheelMove() * 2.0f;
    Vector3 playerMovement = {0.0f, 0.0f, 0.0f};
    Vector3 playerRotation = (Vector3) {
            GetMouseDelta().x*0.05f,                            // Rotation: yaw
            GetMouseDelta().y*0.05f,                            // Rotation: pitch
            0.0f                                              // Rotation: roll
    };

    playerMovement = Vector3Add(playerMovement, getMovementVectorFromInputs());
    playerMovement = Vector3Add(playerMovement, getFallMovement(camera->position, playerPhysics, map));
    playerMovement = Vector3Add(playerMovement, getJumpMovementFromInputs(playerPhysics));
    playerMovement = getCollisionFromMovement(playerMovement, playerRotation, *camera, playerZoom, *playerPhysics, map);
    UpdateCameraPro(camera, playerMovement, playerRotation, playerZoom);
    return 0;
}



Vector3 getMovementVectorFromInputs() {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        playerMovement.x += MOVEMENT_SPEED;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        playerMovement.x -= MOVEMENT_SPEED;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        playerMovement.y -= MOVEMENT_SPEED;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        playerMovement.y += MOVEMENT_SPEED;
    }

    return playerMovement;
}

Vector3 getJumpMovementFromInputs(playerPhysics_t * playerPhysics) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };

    if(IsKeyDown(KEY_SPACE) && !playerPhysics->isJumping && !playerPhysics->isFalling) {
        playerPhysics->isJumping = 1;
    }

    if(playerPhysics->isJumping) {
        if(playerPhysics->jumpTime < JUMP_DURATION) { // Jump up
            playerMovement.z += getJumpSpeed(*playerPhysics);
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

Vector3 getFallMovement(Vector3 playerPosition, playerPhysics_t * playerPhysics, int ** map) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };
    if(playerPhysics->isJumping) {
        return playerMovement;
    }

    float distance = getDistanceFromGround(playerPosition, map);
    // Quand on detecte la chute, on initialise les variables de chute
    if(distance > 0 && !playerPhysics->isFalling) {
        playerPhysics->isFalling = 1;
        playerPhysics->fallingSpeed = FALL_BASE_SPEED;
        playerPhysics->fallTime = 0;
    }
    // Si on est en chute libre, on augmente la vitesse de chute
    else if(distance > 0 && playerPhysics->isFalling) {
        playerPhysics->fallTime++;
        playerPhysics->fallingSpeed = getFallSpeed(*playerPhysics);
    }
    // Si on est au sol, on arrete la chute
    else if(distance == 0 && playerPhysics->isFalling) {
        playerPhysics->isFalling = 0;
        playerPhysics->fallTime = 0;
        playerPhysics->fallingSpeed = 0;
        return playerMovement;
    } else {
        return playerMovement;
    }
    // On calcule la vitesse de chute et on deplace le joueur
    float fallingSpeed = getFallSpeed(*playerPhysics);
    // Si la vitesse de chute est superieure a la distance au sol, on deplace le joueur de la distance au sol
    if(distance >= fallingSpeed) {
        playerPhysics->fallTime++;
        playerPhysics->fallingSpeed = fallingSpeed;
        playerMovement.z -= fallingSpeed;
    // Sinon on deplace le joueur de la vitesse de chute
    } else {
        playerPhysics->isFalling = 0;
        playerPhysics->fallTime = 0;
        playerPhysics->fallingSpeed = 0;
        playerMovement.z -= distance;
    }
    return playerMovement;
}

float getFallSpeed(playerPhysics_t player) {
    return player.fallingSpeed + (player.fallTime * player.fallingSpeed / 100) ;
}

float getJumpSpeed(playerPhysics_t player) {
    return player.jumpingSpeed - (player.jumpTime * player.jumpingSpeed / 100) ;
}

float getDistanceFromGround(Vector3 playerPosition, int ** map) {
    int caseAX = (int)playerPosition.x;
    int caseAZ = (int)playerPosition.z;
    int caseBX;
    int caseBZ;
    int multipleX = 0;
    int multipleZ = 0;
    int maxHeight;

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
    else if((int)playerPosition.x != (int)(playerPosition.z - PLAYER_WIDTH)) {
        multipleZ = 1;
        caseBZ = (int)(playerPosition.z - PLAYER_WIDTH);
    }

    if(multipleX && multipleZ) {
        maxHeight = MAX4(map[caseAX][caseAZ], map[caseAX][caseBZ], map[caseBX][caseAZ], map[caseBX][caseBZ]);
    } else if (multipleX) {
        maxHeight = MAX2(map[caseAX][caseAZ], map[caseBX][caseAZ]);
    } else if (multipleZ) {
        maxHeight = MAX2(map[caseAX][caseAZ], map[caseAX][caseBZ]);
    } else {
        maxHeight = map[(int)playerPosition.x][(int)playerPosition.z];
    }

    return playerPosition.y - (PLAYER_HEIGHT + maxHeight);
}

//TODO
Vector3 getCollisionFromMovement(Vector3 movement, Vector3 playerRotation, Camera camera, float playerZoom, playerPhysics_t playerPhysics, int ** map) {

    int caseAX;
    int caseAZ;
    int caseBX;
    int caseBZ;
    int multipleX = 0;
    int multipleZ = 0;
    Vector3 correctedMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };

    Camera cameraAfterMove = camera;
    UpdateCameraPro(&cameraAfterMove, movement, playerRotation, playerZoom);
    caseAX = (int)cameraAfterMove.position.x;
    caseAZ = (int)cameraAfterMove.position.z;

    if(cameraAfterMove.position.x <= PLAYER_WIDTH
       || cameraAfterMove.position.x + PLAYER_WIDTH >= MAP_SIZE
       || cameraAfterMove.position.z - PLAYER_WIDTH <= 0 ||
       cameraAfterMove.position.z + PLAYER_WIDTH >= MAP_SIZE
            ) {
        return correctedMovement;
    }


    // On verifie que le joueur ne sort pas de la map

    // ON verifie quvalidateMovemente le joueur ne rentre pas dans un mur
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

    //On affiche les coordonnes et le mouvement
    DrawText(TextFormat("X: %f, Y: %f, Z: %f, | X: %f, Y: %f, Z: %f,", cameraAfterMove.position.x, cameraAfterMove.position.y, cameraAfterMove.position.z, movement.x, movement.y, movement.z), 10, 10, 20, RED);
    DrawText(TextFormat("mX: %d, mz: %d", multipleX, multipleZ), 10, 40, 20, RED);

    int isMoveOk = 1;
    for(int i=-1; i < PLAYER_HEIGHT -1; i++) {
        if(multipleX && multipleZ) {
            isMoveOk = isMoveOk && cameraAfterMove.position.y+ i >= map[caseBX][caseBZ];
        } else if (multipleX) {
            isMoveOk = isMoveOk && cameraAfterMove.position.y + i >= map[caseBX][caseAZ];
        } else if (multipleZ) {
            isMoveOk = isMoveOk && cameraAfterMove.position.y + i >= map[caseAX][caseBZ];
        } else {
            isMoveOk = isMoveOk && cameraAfterMove.position.y + i >= map[caseAX][caseAZ];
        }
    }
    if(isMoveOk) {
        return movement;
    }
    correctedMovement.z = movement.z;
    return correctedMovement;
}