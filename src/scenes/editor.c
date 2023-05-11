#include <stdlib.h>
#include <ncurses.h>
#include "menu.h"


/*
 * Initialise une fenêtre de menu
 */
int newEditorScene(EditorScene* win) {

    const int buttonHeight = 3;
    const int buttonWidth = 20;
    const int buttonsMarginX = 6;
    const int buttonsMarginY = 6;
    const int buttonMarginBottom = 2;
    int height, width, ltcx, ltcy, midx, midy;

    height = getmaxy(stdscr);
    width = getmaxx(stdscr);

    //Milieu de l'écran
    midx = width / 2;
    midy = height / 2;

    //Coordonnées du coin supérieur gauche de la fenêtre
    ltcx = midx - MENU_WIN_COLS / 2;
    ltcy = midy - MENU_WIN_ROWS / 2;



    return EXIT_SUCCESS;
}


FILE** getAllSaves() {
    struct dirent *dir;
    DIR * saveDir;
    saveDir = opendir("../bin/");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    return(0);
}

int destroyMenuScene(MenuScene menu) {

}