//
// Created by 20015007@hq.corp.leroymerlin.com on 15/05/23.
//

#include "../../includes/raylib.h"
#include "../board/board.h"
#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#include "../entities.h"
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

void Render(chunkedMap_t);

void toggleDrawCeiling();
void toggleDrawDebug();
void toggleDrawOverlay();

drawBundle_t getDrawBundle();
void setDrawBundle(drawBundle_t);

#endif
