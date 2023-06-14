#ifndef PROJECT_MAIN_MENU
#define PROJECT_MAIN_MENU

void initMenu();
void destroyMenu();

void renderMenu();
void handleMenu();

bool getQuitGameAction();
bool getPlayGameAction();
char * getSaveName();
void getAllSaves();
typedef enum {
	MAIN_MENU_VIEW,
	SAVE_MENU_VIEW,
	GAME_VIEW
} Scenes_e;

void setCurrentScene(Scenes_e scene);

#endif
