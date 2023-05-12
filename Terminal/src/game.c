#include <ncurses.h>
#include <stdlib.h>
<<<<<<< HEAD:Terminal/src/game.c
#include "utils/fonctions.h"
#include "scenes/menu.h"
#include "utils/errorCodes.h"
#include "scenes/editor.h"
#include "game.h"

=======
#include <locale.h>
#include "utils/colors.h"
#include "utils/fonctions.h"
#include "scenes/window.h"
#include "utils/errorCodes.h"
#include "game.h"



>>>>>>> master:src/game.c
game_interface_t interface;


int main(int argc, char* argv[]) {
    ncurses_init();
    newMenuWin(&interface.menu, MENU_OPTIONS);
    menu();
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
    destroyCurrentScene();
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

int edit() {
    newEditorScene(&interface.editor);
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
<<<<<<< HEAD:Terminal/src/game.c
        case EDITOR:
            //destroyEditorScene();
            break;
        default:
            return EXIT_INVALID_SCENE;
    }
}

=======
        default:
            return EXIT_INVALID_SCENE;
    }
}
>>>>>>> master:src/game.c
