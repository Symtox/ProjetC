#include <stdlib.h>
#include <ncurses.h>
#include "window.h"


/*
 * Initialise une fenêtre de menu
 */
int newMenuWin(MenuWin* win, const menuOption_t* buttons) {

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

    //box = bordure de la fenêtre TODO fusionner les feneteres ?
    win->boxwin = newwin(MENU_WIN_ROWS+2, MENU_WIN_COLS+2, ltcy, ltcx);
    win->win = subwin(win->boxwin, MENU_WIN_ROWS, MENU_WIN_COLS, ltcy+1, ltcx+1);

    for(int i = 0; i < BUTTON_COUNT; i++) {
        win->buttons[i] = buttons[i];
        win->buttonWindows[i] = subwin(win->win, buttonHeight, buttonWidth, ltcy + buttonsMarginY + (buttonMarginBottom + buttonHeight) * i, ltcx + buttonsMarginX);
        box(win->buttonWindows[i], 0, 0); // Bordure
        mvwprintw(win->buttonWindows[i], 1, 1, "%s", buttons[i].label);
        wrefresh(win->buttonWindows[i]); // Refresh du composant pour l'afficher
    }

    box(win->boxwin, 0, 0);

    //Refresh de l'affichage
    wrefresh(win->boxwin);
    wrefresh(win->win);

    return EXIT_SUCCESS;
}


int dispatchMenuClick(MEVENT event, MenuWin * win) {
    for(int i = 0; i < BUTTON_COUNT; i++) {
        if(wmouse_trafo(win->buttonWindows[i], &event.y, &event.x, FALSE)) {
            return win->buttons[i].callback();
        }
    }
    return -1;
}