#include "physics.h"
#include "../utils/utils.h"
#include "../../includes/rcamera.h"
#include "../core/renderer.h"
#define APPROX_Y 0.01





int handlePlayerMovement(player_t * player, chunkedMap_t map) {
    drawBundle_t bundle = getDrawBundle();
    Vector3 playerMovement = {0.0f, 0.0f, 0.0f};
    Vector3 playerRotation = getPlayerOrientation();

    playerMovement = Vector3Add(playerMovement, getJumpMovementFromInputs(&player->physics));
    playerMovement = Vector3Add(playerMovement, getNoclipMovement(&player->physics));
    playerMovement = Vector3Add(playerMovement, getMovementVectorFromInputs(player->physics.noclip));

    playerMovement = Vector3Add(playerMovement, getFallMovement(player->camera->position, &player->physics, map));
    correctMovementWithCollisions(&playerMovement, playerRotation, *player->camera, player->physics, map);

    bundle.movement = playerMovement;
    bundle.direction = playerRotation;
    setDrawBundle(bundle);

    updateCameraCustom(player->camera, playerMovement, playerRotation);

    return 0;
}

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

float getMovementSpeed(bool noclip) {
    return noclip ? FREE_WALK_MOVEMENT_SPEED : MOVEMENT_SPEED;
}



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

Vector3 getPlayerOrientation() {
    if(IsGamepadAvailable(0)) {
        return (Vector3){ GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * PLAYER_SENSITIVITY_GAMEPAD, GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * PLAYER_SENSITIVITY_GAMEPAD, 0.0f};
    }
    return (Vector3){GetMouseDelta().x * PLAYER_SENSITIVITY_MOUSE, GetMouseDelta().y * PLAYER_SENSITIVITY_MOUSE, 0.0f};
}

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
    } else {
        logFile(TextFormat("Falling: Jump time: %d\n", playerPhysics->jumpTime));
    }

    return playerMovement;
}

void handleJump(playerPhysics_t * playerPhysics) {
    if(IsKeyPressed(KEY_SPACE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
        if(!playerPhysics->isJumping && !playerPhysics->isFalling) {
            playerPhysics->isJumping = 1;
        }
    }

}

Vector3 getFallMovement(Vector3 playerPosition, playerPhysics_t * playerPhysics, chunkedMap_t map) {
    Vector3 playerMovement = (Vector3) {
            0.0f,                            // Movement: sideways
            0.0f,                            // Movement: up-down
            0.0f                            // Movement: forward-back
    };


    float distance = getPlayerDistanceFromGround(playerPosition, map);


    if(playerPhysics->isJumping || distance == 0 || playerPhysics->noclip) {
        return playerMovement;
    }

    float fallingSpeed = getFallSpeed(*playerPhysics);
    logFile(TextFormat("Falling: Distance: %f, x: %f, y: %f, z: %f\n", distance, playerPosition.x, playerPosition.y, playerPosition.z));

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

    logFile(TextFormat("Falling: Max height: %f\n", minDistance));
    return minDistance - PLAYER_HEIGHT;
}

void correctMovementWithCollisions(Vector3 * movement, Vector3 playerRotation, Camera camera, playerPhysics_t playerPhysics, chunkedMap_t map) {

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

    //correctedMovement.x = Vector3Subtract(cameraAfterMove.position, camera.position).x;
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

float getTileDistanceFromGround(int x, float y, int z, chunkedMap_t map) {
    int chunkIndexX, chunkIndexY;
    if(x < 0 || z < 0 || x >= CHUNK_SIZE * MAP_CHUNK_WIDTH || z >= CHUNK_SIZE * MAP_CHUNK_HEIGHT || y < 0 || y >= MAX_Y) {
        return 10;
    }
    toChunkCoords(&x, &z, &chunkIndexX, &chunkIndexY, map);
    if(chunkIndexX == -1 || chunkIndexY == -1) {
        return 10;
    }


    for(int i=y - PLAYER_HEIGHT; i >= 0; i--) {
        if(map.chunks[chunkIndexX][chunkIndexY].chunk[x][i][z] != 0) {
            return y - i;
        }
    }


    return y;
}

int isTileFree(int x, int y, int z, chunkedMap_t map) {
    int chunkIndexX, chunkIndexY;
    bool isDoorFree = true;
    bool isMonsterFree = true;

    if(x < 0 || z < 0 || x >= CHUNK_SIZE * map.maxX || z >= CHUNK_SIZE * map.maxY || y < 0 || y >= MAX_Y) {
        return 1;
    }
    toChunkCoords(&x, &z, &chunkIndexX, &chunkIndexY, map);
    if(chunkIndexX == -1 || chunkIndexY == -1) {
        return 1;
    }

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

    return map.chunks[chunkIndexX][chunkIndexY].chunk[x][y][z] == 0
        && isDoorFree
        && isMonsterFree;
}

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