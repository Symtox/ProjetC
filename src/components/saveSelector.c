#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <dirent.h>
#include "saveSelector.h"

int newSaveSelector(SaveSelector_c* component, int width, int x, int y) {

    component->noFiles = scandir("../bin", &component->files, filterSaveFiles, alphasort);

    component->win = newwin(2*(component->noFiles)+2, width, x, y);

    box(component->win, 0, 0);
    mvwprintw(component->win, 1, 3, "Choisir une sauvegarde");


    //Affiche les sauvegardes
    for(int i=0; i<component->noFiles; i++) {
        mvwprintw(component->win, (i+1) * 2, 3, "%s", component->files[i]->d_name);
    }

    wrefresh(component->win);
    refresh();
    return EXIT_SUCCESS;
}

int filterSaveFiles(const struct dirent * file) {
    return strcmp(file->d_name, ".") || strcmp(file->d_name, "..") ? 0 : 1;
}
