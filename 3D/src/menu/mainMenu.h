#ifndef PROJECT_MAIN_MENU
#define PROJECT_MAIN_MENU

void initMenu();
void renderMainMenu();
void handleMenu();


bool getQuitGameAction();
bool getPlayGameAction();

typedef enum {
	MAIN_MENU_VIEW,
	SAVE_MENU_VIEW,
	GAME_VIEW
} Scenes_e;

#endif
