#define BUTTON_COUNT 4

#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H


    #define MENU_WIN_COLS 60
    #define MENU_WIN_ROWS 30

    /*
     * Structure de données pour les boutons du menu
     */
    typedef struct {
        char label[10];
        int (*callback)();
    } menuOption_t;

    /*
     * Structure de données pour le menu
     */
    /*typedef struct {
        WINDOW * boxwin, * win;
        WINDOW * buttonWindows[BUTTON_COUNT];
        menuOption_t buttons[BUTTON_COUNT];
    }MenuScene;

    typedef struct {
        WINDOW * gameWindow;
    } GameScene;*/


    int newMenuWin(MenuScene *, const menuOption_t *);
    int dispatchMenuClick(MEVENT, MenuScene *);
    int destroyMenuScene(MenuScene);
#endif
