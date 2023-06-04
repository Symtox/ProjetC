//
// Created by 20015007@hq.corp.leroymerlin.com on 15/05/23.
//

#include "../../includes/raylib.h"
#include "../board/board.h"
#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#include "../board/entities.h"
typedef struct {
    int drawCeiling;
    int drawDebug;
    int drawOverlay;
    Vector3 movement;
    Vector3 direction;
    player_t * player;
    int tileUnder;

}drawBundle_t;

void initRenderer(player_t * player);

void DrawMap(chunkedMap_t);
void DrawChunk(chunk_t);
void DrawOverlay();
void DrawDebug();
void DrawCubeCustom(Texture2D blockTexture, Vector3 position, float width, float height, float length, Color color);

void Render(chunkedMap_t);

void toggleDrawCeiling();
void toggleDrawDebug();
void toggleDrawOverlay();

drawBundle_t getDrawBundle();
void setDrawBundle(drawBundle_t);

int render3DText(char * , Vector3 , float);
void DrawKey(DoorKey_t);
void DrawPotion(potion_t,int,int);
// void DrawPowerUp(powerUp_t);
// void DrawDoor(door_t);
// void DrawMonster(monster_t);
// void DrawPlayer(player_t);


#endif
