#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include "utils/colors.h"
#include "utils/fonctions.h"
#include "scenes/window.h"
#include "utils/errorCodes.h"
#include "game.h"



game_interface_t interface;


int main(int argc, char* argv[]) {
    init();
    init_menu();
    menu();
    return EXIT_SUCCESS;
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
int init_menu() {
    newMenuWin(&interface.menu, MENU_OPTIONS);
    return EXIT_SUCCESS;
}

int menu() {
    int inputChar;
    MEVENT event;
    while ((inputChar = getch()) != 'q') {
        if(inputChar == KEY_MOUSE && getmouse(&event) == OK) { // Si l'action est une action de souris
            switch(dispatchMenuClick(event, &interface.menu)) {
                case EXIT_TO_MENU:
                case EXIT_END_GAME:
                    menuToFirstScene();
                    break;
                case EXIT_QUIT_GAME:
                    cleanAndQuit();
                    break;
            }
        }
    }
    cleanAndQuit();
    return EXIT_SUCCESS;
}

/*
 * Nettoyage de l'interface et sortie du programme
 * TODO free memory
 */
int cleanAndQuit() {
    delwin(interface.menu.boxwin);
    delwin(interface.menu.win);
    for(int i = 0; i < BUTTON_COUNT; i++) {
        delwin(interface.menu.buttonWindows[i]);
    }
    ncurses_stop();
    exit(0);
}

int menuToFirstScene() {
    destroyCurrentScene();
    return 0;//TODO
}

int play() {
    destroyCurrentScene();
    return 0;//TODO
}

int options() {
    return 0;//TODO
}

int quit() {
    return EXIT_QUIT_GAME;
}

int destroyCurrentScene() {
    switch(interface.currentScene) {
        case MENU:
            return destroyMenuScene(interface.menu);
        case GAME:
            //destroyGameScene();
            break;
        case OPTIONS:
            //destroyOptionsScene();
            break;
        default:
            return EXIT_INVALID_SCENE;
    }
}