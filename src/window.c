#include <stdlib.h>
#include <ncurses.h>
#include "window.h"


#define MENU_WIN_COLS 50
#define MENU_WIN_ROWS 30



int newMenuWin(MenuWin* win, int a, int b, int c, int d) {
    win->boxwin = newwin(MENU_WIN_ROWS+2, MENU_WIN_COLS+2, 5, 5);
    win->win = subwin(win->boxwin, MENU_WIN_ROWS, MENU_WIN_COLS, 6, 6);

    box(win->boxwin, 0, 0);
    mvwprintw(win->win, 0, 0, "Pressez plusieurs fois une touche pour passer a l'etape suivante");

    wrefresh(win->boxwin);
    wrefresh(win->win);

    return EXIT_SUCCESS;
}
