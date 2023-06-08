//
// Created by 20015007@hq.corp.leroymerlin.com on 15/05/23.
//

#include "../../includes/raylib.h"
#include "../board/board.h"
#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#include "../board/entities.h"

typedef struct {
    char text[200];
    int choiceCount;
    const char * choices[2];
    KeyboardKey keys[2];
}dialog_t;

typedef struct {
    int drawCeiling;
    int drawDebug;
    int drawOverlay;
    Vector3 movement;
    Vector3 direction;
    player_t * player;
    int tileUnder;
    int canOpenDoor;
    int canOpenFight;
    dialog_t dialog;
}drawBundle_t;

Texture2D getKeyTexture(KeyboardKey key);

void initRenderer(player_t * player);
void DrawFightDialog();
void DrawDoorHint();
void DrawMap(chunkedMap_t);
void DrawChunk(chunk_t);
void DrawOverlay();
void DrawDebug();
void DrawCubeCustom(Texture2D blockTexture, Vector3 position, float width, float height, float length, Color color);
void DrawFightHint();
void Render(chunkedMap_t);

void toggleDrawCeiling();
void toggleDrawDebug();
void toggleDrawOverlay();

drawBundle_t getDrawBundle();
void setDrawBundle(drawBundle_t);

int render3DText(const char * , Vector3 , float);
void DrawKey(DoorKey_t);
void DrawPotion(potion_t, int ,int);
void DrawPowerUp(powerUp_t, int, int);
void DrawHealth(powerUp_t health, int x, int y);
void DrawDoor(door_t, int, int);
void DrawMonster(monster_t, int, int);

#endif
