#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include "colors.h"
#include "fonctions.h"
#include "window.h"

game_interface_t interface;


int init_game();
int init();

int main(int argc, char* argv[]) {
    init();
    init_game();
}

int init() {
    setlocale(LC_ALL, "");
    ncurses_init();
    ncurses_initsouris();
    ncurses_couleurs();
    palette();
    bkgd(COLOR_PAIR(PAIR_BASIC));
    refresh();
    return EXIT_SUCCESS;
}


//Initialisation et affichage de l'interface
int init_game() {
    newMenuWin(&interface.info, 10, 10, 30, 30);
    wrefresh(stdscr);
    getch();
    ncurses_stop();
    return EXIT_SUCCESS;
}


