#define BUTTON_COUNT 3

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
    typedef struct {
        WINDOW * boxwin, * win;
        WINDOW * buttonWindows[BUTTON_COUNT];
        menuOption_t buttons[BUTTON_COUNT];
    }MenuWin;

    typedef struct  {
        MenuWin menu;
    } game_interface_t;

    int newMenuWin(MenuWin *, const menuOption_t *);
    int dispatchButtonsClick(int);
#endif
