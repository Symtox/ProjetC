#ifndef _FONCTIONS_
#define _FONCTIONS_

/**
 * Initialisation de ncurses.
 */
void ncurses_init();

/**
 * Fin de ncurses.
 */
void ncurses_stop();

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs();

/**
 * Initialisation de la souris.
 */
void ncurses_initsouris();

/**
 * Récupération de la position de la souris.
 */
int souris_getpos(int *x, int *y);

#endif