#define BUTTON_COUNT 3

#ifndef PROJECT_EDITOR
#define PROJECT_EDITOR

/*
 * Structure de données pour le menu
 */
typedef struct {
    WINDOW * boxwin, * win;
}MenuScene;

int newMenuWin(MenuScene *, const menuOption_t *);
int dispatchMenuClick(MEVENT, MenuScene *);
int destroyMenuScene(MenuScene);

#endif
