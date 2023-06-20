
#include "../../includes/raylib.h"
#include "../board/board.h"
#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H
#include "../board/entities.h"
#include "../utils/utils.h"

/**
 * Dialog de combat
 */
typedef struct {
    char text[200]; //Main text
    int choiceCount; // Nombre d'actions possibles
    const char * choices[2]; // Texte lié au choix
    char keys[2]; // Touche lié au choix TODO
}dialog_t;


/**
 * Options d'affichage
 */
typedef struct {
    int drawCeiling; //Plafond (c)
    int drawDebug; //Debug (f3)
    int drawOverlay; //Overlay (f1)
    int paused; //Pause menu
    Vector3 movement; // debug info (movement)
    Vector3 direction; // debug info (direction)
    player_t * player; // toutes les info du joueur en temps réel
    int canOpenDoor; // Pour les indices d'ouverture de porte
    int canOpenFight; // Pour les indice de début de combat
    int win;
    dialog_t dialog; // Dialog lié au combat
    int godMode;
    int bulletCount;
    bullet_t * bullets;
    int grenadeCount;
    grenade_t * grenades;
}drawBundle_t;

Texture2D getKeyTexture(KeyboardKey key);

void initRenderer(player_t * player);
void DrawEndGameTile(int x, int y);
void DrawWinScreen();
void DrawEndScreen();
void DrawFightDialog();
void DrawDoorHint();
void DrawMap(chunkedMap_t);
void DrawChunk(chunk_t);
void DrawOverlay();
void DrawDebug();
void drawGodMod();
void DrawCubeCustom(Texture2D blockTexture, Vector3 position, float width, float height, float length, Color color);
void DrawFightHint();
void Render(chunkedMap_t);

void toggleDrawCeiling();
void toggleDrawDebug();
void toggleDrawOverlay();

drawBundle_t getDrawBundle();
void setDrawBundle(drawBundle_t);
void drawBullets();

int render3DText(const char * , Vector3 , float);
void DrawKey(DoorKey_t, int, int);
void DrawPotion(potion_t, int ,int);
void DrawPowerUp(powerUp_t, int, int);
void DrawHealth(powerUp_t health, int x, int y);
void DrawDoor(door_t, int, int);
void DrawMonster(monster_t, int, int);
void destroyRenderer();
void drawGrenades();
void DrawPauseMenu();
Rectangle getPauseMenuResumeButtonBounds();
Rectangle getPauseMenuQuitButtonBounds();

#endif
