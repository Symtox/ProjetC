#include <ncurses.h>
#include <stdlib.h>
#include "colors.h"

//TODO Comment + changer la palette
int palette() {
    int ret = EXIT_FAILURE;

    if(can_change_color()) {

        init_color(COLOR_GREY, 500, 500, 500);
        init_color(COLOR_WHITE_SNOW, 800, 800, 800);
        init_color(COLOR_BLUE_ICE, 500, 800, 900);
        init_color(COLOR_BLACK_GREEN, 0, 255, 0);
        init_color(COLOR_BROWN, 230, 200, 150);
        init_pair(PAIR_GREY, COLOR_BLACK, COLOR_GREY);
        init_pair(PAIR_WHITE_SNOW, COLOR_BLACK, COLOR_WHITE_SNOW);
        init_pair(PAIR_BROWN, COLOR_BLACK, COLOR_BROWN);
        init_pair(PAIR_BLACK_GREEN, COLOR_BLACK, COLOR_BLACK_GREEN);
        init_pair(PAIR_BLUE_ICE, COLOR_BLACK, COLOR_BLUE_ICE);
        
        ret = EXIT_SUCCESS;
        
    } else {
        init_pair(PAIR_GREY, COLOR_BLACK, COLOR_RED);
    }

    init_pair(PAIR_BASIC, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_BLUE, COLOR_BLACK, COLOR_BLUE);
    init_pair(PAIR_GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(PAIR_YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(PAIR_RED, COLOR_BLACK, COLOR_RED);

    return ret;
}