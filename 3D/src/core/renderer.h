//
// Created by 20015007@hq.corp.leroymerlin.com on 15/05/23.
//

#include "../../includes/raylib.h"
#include "../board/board.h"
#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H

void DrawMap(map_t);
void DrawChunk(chunk_t);
void setDrawCeiling(bool);

void DrawOverlay(Camera);
void Render(map_t, Camera);

#endif