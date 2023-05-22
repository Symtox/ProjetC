#include "physics.h"
#include "../utils/utils.h"
#include "../../includes/rcamera.h"
#include "../board/board.h"
#include "../board/tiles.h"
#define APPROX_Y 0.01

int controlsToggles[1] = {0};

typedef enum controls {
    FREE_WALK = 0
}controls_e;

int handlePlayerMovement(Camera *camera, playerPhysics_t * playerPhysics, map_t map) {
    Vector3 playerMovement = {0.0f, 0.0f, 0.0f};
    Vector3 playerRotation = (Vector3) {
            GetMouseDelta().x * PLAYER_SENSITIVITY,                            // Rotation: yaw
            GetMouseDelta().y * PLAYER_SENSITIVITY,                            // Rotation: pitch
            0.0f                                              // Rotation: roll
    };




    playerMovement = Vector3Add(playerMovement, getMovementVectorFromInputs(playerPhysics->freeWalk));
    playerMovement = Vector3Add(playerMovement, getFallMovement(camera->position, playerPhysics, map));
    playerMovement = Vector3Add(playerMovement, getJumpMovementFromInputs(playerPhysics));
    playerMovement = Vector3Add(playerMovement, getFreeWalkMovement(playerPhysics));

    correctMovementWithCollisions(&playerMovement, playerRotation, *camera, *playerPhysics, map);
    updateCameraCustom(camera, playerMovement, playerRotation);

    return 0;
}

Vector3 getFreeWalkMovement(playerPhysics_t * playerPhysics) {
    if(IsKeyDown(KEY_F) && !controlsToggles[FREE_WALK]) {
        playerPhysics->freeWalk = !playerPhysics->freeWalk;
        controlsToggles[FREE_WALK] = 1;
    }
    if(IsKeyUp(KEY_F) && controlsToggles[FREE_WALK]) {
        controlsToggles[FREE_WALK] = 0;
    }

    Vector3 playerMovement = {0.0f, 0.0f, 0.0f};
    if(IsKeyDown(KEY_SPACE) && playerPhysics->freeWalk) {
        playerMovement.y += FREE_WALK_MOVEMENT_SPEED;
    }
    if(IsKeyDown(KEY_LEFT_SHIFT) && playerPhysics->freeWalk) {
        playerMovement.y -= FREE_WALK_MOVEMENT_SPEED;
    }
    return playerMovement;

}

float getMovementSpeed(bool freeWalk) {
    return freeWalk ? FREE_WALK_MOVEMENT_SPEED : MOVEMENT_SPEED;
}



Vector3 getMovementVectorFromInputs(bool freeWalk) {
    Vector3 playerMovement = (Vector3) {
            (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) - (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)),
            0.0f,
            (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) - (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    };
    return Vector3Scale(Vector3Normalize(playerMovement), getMovementSpeed(freeWalk));
}


Vector3 getJumpMovementFromInputs(playerPhysics_t * playerPhysics) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };

    if(playerPhysics->freeWalk) {
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

void handleJump(playerPhysics_t * playerPhysics) {
    if(IsKeyPressed(KEY_SPACE) && !playerPhysics->isJumping && !playerPhysics->isFalling) {
        playerPhysics->isJumping = 1;
    }
}

Vector3 getFallMovement(Vector3 playerPosition, playerPhysics_t * playerPhysics, map_t map) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };


    float distance = getDistanceFromGround(playerPosition, map);

    if(playerPhysics->isJumping || distance == 0 || playerPhysics->freeWalk) {
        return playerMovement;
    }

    float fallingSpeed = getFallSpeed(*playerPhysics);

    logFile(TextFormat("Falling: Falling speed: %f %d\n", fallingSpeed, playerPhysics->fallTime));
    playerPhysics->isFalling = 1;
    if(distance > fallingSpeed) {
        playerPhysics->fallTime += 1;
        playerPhysics->fallingSpeed = fallingSpeed;
        playerMovement.y -= fallingSpeed;
    } else {
        playerPhysics->isFalling = 0;
        playerPhysics->fallTime = 0;
        playerPhysics->fallingSpeed = FALL_BASE_SPEED;
        playerMovement.y -= distance;
    }
    return playerMovement;
}

float getFallSpeed(playerPhysics_t player) {
    return MIN2(-1 + pow(3, (float)player.fallTime / 50), MAX_FALL_SPEED);
}

float getJumpSpeed(playerPhysics_t player) {
    return player.jumpingSpeed - (player.jumpTime * player.jumpingSpeed / 100) ;
}

float getDistanceFromGround(Vector3 playerPosition, map_t map) {
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
    else if((int)playerPosition.z != (int)(playerPosition.z - PLAYER_WIDTH)) {
        multipleZ = 1;
        caseBZ = (int)(playerPosition.z - PLAYER_WIDTH);
    }

    logFile(TextFormat("Player position: %f %f\n", playerPosition.x, playerPosition.z));
    if(multipleX && multipleZ) {
        logFile(TextFormat("multipleX && multipleZ: case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map), caseBX, caseBZ, getTileFromCoordsAndMap(caseBX, caseBZ, map)));
    }
    else if(multipleX) {
        logFile(TextFormat("multipleX: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map)));
    }
    else if(multipleZ) {
        logFile(TextFormat("multipleZ: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map)));
    }
    else {
        logFile(TextFormat("case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map)));
    }
    if(multipleX && multipleZ) {
        DrawText(TextFormat("multipleX && multipleZ: case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map), caseBX, caseBZ, getTileFromCoordsAndMap(caseBX, caseBZ, map)), 10, 10, 20, MAROON);
        maxHeight = MAX4(getTileFromCoordsAndMap(caseAX, caseAZ, map), getTileFromCoordsAndMap(caseAX, caseBZ, map), getTileFromCoordsAndMap(caseBX, caseAZ, map), getTileFromCoordsAndMap(caseBX, caseBZ, map));
    } else if (multipleX) {
        DrawText(TextFormat("multipleX: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map)), 10, 10, 20, MAROON);
        maxHeight = MAX2(getTileFromCoordsAndMap(caseAX, caseAZ, map), getTileFromCoordsAndMap(caseBX, caseAZ, map));
    } else if (multipleZ) {
        DrawText(TextFormat("multipleZ: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map)), 10, 10, 20, MAROON);
        maxHeight = MAX2(getTileFromCoordsAndMap(caseAX, caseAZ, map), getTileFromCoordsAndMap(caseAX, caseBZ, map));
    } else {
        DrawText(TextFormat("case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map)), 10, 10, 20, MAROON);
        maxHeight = getTileFromCoordsAndMap(playerPosition.x, playerPosition.z, map);
    }

    return playerPosition.y - (PLAYER_HEIGHT + maxHeight);
}

//TODO
void correctMovementWithCollisions(Vector3 * movement, Vector3 playerRotation, Camera camera, playerPhysics_t playerPhysics, map_t map) {

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
/*
    if(cameraAfterMove.position.x <= PLAYER_WIDTH
       || cameraAfterMove.position.x + PLAYER_WIDTH >= CHUNK_SIZE
       || cameraAfterMove.position.z - PLAYER_WIDTH <= 0 ||
       cameraAfterMove.position.z + PLAYER_WIDTH >= CHUNK_SIZE
            ) {
        movement->x = 0;
        movement->z = 0;
        return;
    }

*/
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

    logFile(TextFormat("Collision:\n before: %f, %f, %f\n after: %f, %f, %f\n", camera.position.x, camera.position.y, camera.position.z, cameraAfterMove.position.x, cameraAfterMove.position.y, cameraAfterMove.position.z));
    if(multipleX && multipleZ) {
        logFile(TextFormat("multipleX && multipleZ: case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map), caseBX, caseBZ, getTileFromCoordsAndMap(caseBX, caseBZ, map)));
    }
    else if(multipleX) {
        logFile(TextFormat("multipleX: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map)));
    }
    else if(multipleZ) {
        logFile(TextFormat("multipleZ: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map)));
    }


    int isMoveOk = 1;
    for(int i=0; i < PLAYER_HEIGHT; i++) {
        if(multipleX && multipleZ) {
            isMoveOk = isMoveOk && (cameraAfterMove.position.y - PLAYER_HEIGHT + i) >= getTileFromCoordsAndMap(caseBX, caseBZ, map);
        }
        if (multipleX) {
            isMoveOk = isMoveOk && (cameraAfterMove.position.y - PLAYER_HEIGHT + i) >= getTileFromCoordsAndMap(caseBX, caseAZ, map);
        }
        if (multipleZ) {
            isMoveOk = isMoveOk && (cameraAfterMove.position.y - PLAYER_HEIGHT + i) >= getTileFromCoordsAndMap(caseAX, caseBZ, map);
        }
        isMoveOk = isMoveOk && (cameraAfterMove.position.y - PLAYER_HEIGHT + i) >= getTileFromCoordsAndMap(caseAX, caseAZ, map);

    }

    logFile(TextFormat("Collision:\n before: %f, %f, %f\n after: %f, %f, %f\n", camera.position.x, camera.position.y, camera.position.z, cameraAfterMove.position.x, cameraAfterMove.position.y, cameraAfterMove.position.z));
    if(multipleX && multipleZ) {
        logFile(TextFormat("multipleX && multipleZ: case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map), caseBX, caseBZ, getTileFromCoordsAndMap(caseBX, caseBZ, map)));
    }
    else if(multipleX) {
        logFile(TextFormat("multipleX: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseBX, caseAZ, getTileFromCoordsAndMap(caseBX, caseAZ, map)));
    }
    else if(multipleZ) {
        logFile(TextFormat("multipleZ: case[%d][%d]: %d case[%d][%d]: %d\n", caseAX, caseAZ, getTileFromCoordsAndMap(caseAX, caseAZ, map), caseAX, caseBZ, getTileFromCoordsAndMap(caseAX, caseBZ, map)));
    }
    if(isMoveOk) {
        return;
    }

    //correctedMovement.x = Vector3Subtract(cameraAfterMove.position, camera.position).x;
   // correctedMovement.x = movement.x;
    movement->x = 0;
    movement->z = 0;
}

void updateCameraCustom(Camera * camera, Vector3 movement, Vector3 rotation) {

    // Required values
    // movement.x - Move forward/backward
    // movement.y - Move right/left
    // movement.z - Move up/down
    // rotation.x - yaw
    // rotation.y - pitch
    // rotation.z - roll
    // zoom - Move towards target

    bool lockView = true;
    bool rotateAroundTarget = false;
    bool rotateUp = false;
    bool moveInWorldPlane = true;

    // Camera rotation
    CameraPitch(camera, -rotation.y*DEG2RAD, lockView, rotateAroundTarget, rotateUp);
    CameraYaw(camera, -rotation.x*DEG2RAD, rotateAroundTarget);
    //CameraRoll(camera, rotation.z*DEG2RAD);

    // Camera movement
    CameraMoveForward(camera, movement.x, moveInWorldPlane);
    CameraMoveRight(camera, movement.z, moveInWorldPlane);
    CameraMoveUp(camera, movement.y);

    // Zoom target distance
}

int getTileFromCoordsAndMap(int x, int y, map_t map) {
    if(x < 0 || y < 0 || x >= CHUNK_SIZE * MAP_SIZE || y >= CHUNK_SIZE * MAP_SIZE) {
        return 0;
    }
    int playerChunkX = (int)(x / CHUNK_SIZE);
    int playerChunkY = (int)(y / CHUNK_SIZE);
    int tileX = (int)(x - playerChunkX * CHUNK_SIZE);
    int tileY = (int)(y - playerChunkY * CHUNK_SIZE);

    for(int i=0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if(map.chunks[i][j].x != -1 && map.chunks[i][j].y != -1 && map.chunks[i][j].x == playerChunkX && map.chunks[i][j].y == playerChunkY) {
                return getHeightFromTileType(map.chunks[i][j].chunk[tileX][tileY]);
            }
        }
    }
    return 0;
}