#ifndef PROJECT_GAME
#define PROJECT_GAME

typedef enum {
    MENU,
    GAME,
    OPTIONS
} scenes_e;

typedef struct  {
    MenuScene menu;
    GameScene game;
    EditorScene editor;
    scenes_e currentScene;
} game_interface_t;





int init_menu();
int init();
int springClean();
int menuToFirstScene();
int menu();
int cleanAndQuit();
int play();
int options();
int quit();
int destroyCurrentScene();
int destroyGameScene();
int edit();

// Boutons du menu et leurs onCLick
const menuOption_t MENU_OPTIONS[] = {
        {"Play", play},
        {"Options", options},
        {"Edit", edit}
        {"Quit", quit}
};

#endif