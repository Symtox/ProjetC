#include <stdlib.h>
#include <ncurses.h>
#include "menu.h"
#include "editor.h"
#include <string.h>
#include "../components/saveSelector.h"


/*
 * Initialise une fenêtre de menu
 */
int newEditorScene(EditorScene* win) {

    int height, width, ltcx, ltcy, midx, midy;
    height = getmaxy(stdscr);
    width = getmaxx(stdscr);

    //Milieu de l'écran
    midx = width / 2;
    midy = height / 2;


    //Coordonnées du coin supérieur gauche de la fenêtre
    ltcx = midx - MENU_WIN_COLS / 2;
    ltcy = midy - MENU_WIN_ROWS / 2;

    newSaveSelector(&win->saveSelector, 20, ltcx, ltcy );
    return EXIT_SUCCESS;
}

int destroyEditorScene(EditorScene scene) {
    delwin(scene.boxwin);
    delwin(scene.win);
}