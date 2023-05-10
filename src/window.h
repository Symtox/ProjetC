#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

    typedef struct {
        WINDOW * boxwin, * win;
    }MenuWin;

    typedef struct  {
        MenuWin info;
    } game_interface_t;

    int newMenuWin(MenuWin *, int, int, int, int);

#endif
