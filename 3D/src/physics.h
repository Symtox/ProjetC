#include "../includes/raylib.h"
#include "../includes/raymath.h"
#include "./Const.h"


#ifndef PROJECT_PHYSICS_H
    #define PROJECT_PHYSICS_H
#define INIT_PLAYER_PHYSICS (playerPhysics_t) {0, 0, 0, 0, FALL_BASE_SPEED, JUMP_BASE_SPEED}

    typedef struct {
        int isJumping;
        int isFalling;
        int jumpTime;
        int fallTime;
        float fallingSpeed;
        float jumpingSpeed;
    }playerPhysics_t;

    int handlePlayerMovement(Camera*, playerPhysics_t *, int **);

    Vector3 getMovementVectorFromInputs();
    Vector3 getJumpMovementFromInputs(playerPhysics_t*);
    Vector3 getFallMovement(Vector3, playerPhysics_t*, int **);
    Vector3 getCorrectedPlayerMovement(Vector3, Vector3, playerPhysics_t, int **);

    playerPhysics_t initPlayerPhysics();

    float getFallSpeed(playerPhysics_t);
    float getJumpSpeed(playerPhysics_t);

    float getDistanceFromGround(Vector3, int**);
    Vector3 getCollisionFromMovement(Vector3 , Vector3 , Camera , float , playerPhysics_t , int ** );


#endif