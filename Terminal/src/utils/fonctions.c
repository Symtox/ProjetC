#include "fonctions.h"

#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include "colors.h"
/**
 * Initialisation de ncurses.
 */

void ncurses_init() {
    setlocale(LC_ALL, "");
    initscr();	        // Démarre le mode ncurses
    cbreak();	            // Désactive la mise en buffer
    noecho();             // Désactive l'affichage des caractères saisis
    keypad(stdscr, TRUE);	// Active les touches spécifiques (flèches)
    refresh();            // Met à jour l'affichage
    curs_set(FALSE);      // Masque le curseur

    ncurses_initsouris();
    ncurses_couleurs();
    palette();
    bkgd(COLOR_PAIR(PAIR_BASIC));
    refresh();
}

void ncurses_stop() {
  endwin();
}

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs() {
  if(has_colors() == FALSE) {
    ncurses_stop();
    fprintf(stderr, "Le terminal ne supporte pas les couleurs.\n");
    exit(EXIT_FAILURE);
  }
  start_color();
}

/**
 * Initialisation de la souris.
 */
void ncurses_initsouris() {
  if(!mousemask(BUTTON1_PRESSED, NULL)) {
    ncurses_stop();
    fprintf(stderr, "Pas de gestion de la souris.\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Récupération de la position de la souris.
 */
int souris_getpos(int *x, int *y) {
  MEVENT event;
  int resultat = getmouse(&event);
  if(resultat == OK) {
    *x = event.x;
    *y = event.y;
  }
  return resultat;
}