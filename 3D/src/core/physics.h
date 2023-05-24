#include "../../includes/raylib.h"
#include "../../includes/raymath.h"
#include "../utils/const.h"
#include "../board/board.h"
#include "../entities.h"

#ifndef PROJECT_PHYSICS_H
    #define PROJECT_PHYSICS_H
#define INIT_PLAYER_PHYSICS (playerPhysics_t) {0, 0, 0, 0, FALL_BASE_SPEED, JUMP_BASE_SPEED}



    int handlePlayerMovement(player_t *, chunkedMap_t);
    void handleJump(playerPhysics_t *);

    Vector3 getMovementVectorFromInputs(bool noclip);
    Vector3 getJumpMovementFromInputs(playerPhysics_t*);
    Vector3 getFallMovement(Vector3, playerPhysics_t*, chunkedMap_t);
    Vector3 getCorrectedPlayerMovement(Vector3, Vector3, playerPhysics_t, chunkedMap_t);

    playerPhysics_t initPlayerPhysics();

    float getFallSpeed(playerPhysics_t);
    float getJumpSpeed(playerPhysics_t);
    float getMovementSpeed(bool);


    void updateCameraCustom(Camera *, Vector3, Vector3);

    float getDistanceFromGround(Vector3, chunkedMap_t);
    void correctMovementWithCollisions(Vector3 *, Vector3 , Camera , playerPhysics_t , chunkedMap_t );

    Vector3 getNoclipMovement(playerPhysics_t *);
    int getTileFromCoordsAndMap(int, int, chunkedMap_t);



#endif