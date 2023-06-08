#include "../../includes/raylib.h"
#include "../utils/const.h"
#include "mainMenu.h"

//////////////Varibales
bool quitBtnAction = false;
bool playBtnAction = false;
bool isInMenu;


Texture2D playButton;
Texture2D quitButton;
Texture2D creditButton;
Texture2D logo;

Texture2D saveMenuBackground;
Texture2D saveButtonBackground;
Texture2D newGameButton;

Rectangle playBtnBounds;
Rectangle creditBtnBounds;
Rectangle quitBtnBounds;
Rectangle logoBounds;
Rectangle newGameButtonBounds;

Rectangle sourceLogoRec;
Rectangle sourcePlayBtnRec;
Rectangle sourceCreditBtnRec;
Rectangle sourceQuitBtnRec;

Vector2 mousePoint;

Scenes_e currentScene = MAIN_MENU_VIEW;
//////////////

void initMenu() {
	playButton = LoadTexture("assets/menu/PlayButton.png");
	creditButton = LoadTexture("assets/menu/CreditButton.png");
	quitButton = LoadTexture("assets/menu/QuitButton.png");
	logo = LoadTexture("assets/menu/mazeSlayerLogo.png");
    saveMenuBackground = LoadTexture("assets/menu/SaveMenu.png");
    newGameButton = LoadTexture("assets/menu/newGameBtn.png");



    playBtnBounds = (Rectangle){ WINDOW_WIDTH/2.0f - playButton.width/2.0f, 400, (float)playButton.width, playButton.height };
    creditBtnBounds = (Rectangle){ WINDOW_WIDTH/2.0f - creditButton.width/2.0f, 520 , (float)creditButton.width, creditButton.height };
    quitBtnBounds = (Rectangle){ WINDOW_WIDTH/2.0f - quitButton.width/2.0f, 640, (float)quitButton.width, quitButton.height };
    logoBounds = (Rectangle){WINDOW_WIDTH/50.0f - logo.width/10.0f,-360, (float)logo.width, logo.height};

    newGameButtonBounds = (Rectangle){ WINDOW_WIDTH * 0.05, WINDOW_HEIGHT * 0.87, (float)newGameButton.width, newGameButton.height };

    sourceLogoRec = (Rectangle){0, 0, logo.width, logo.height};
    sourcePlayBtnRec = (Rectangle){ 0, 0, (float)playButton.width, playButton.height };
    sourceCreditBtnRec = (Rectangle){ 0, 0, (float)creditButton.width, creditButton.height };
    sourceQuitBtnRec = (Rectangle){ 0, 0, (float)quitButton.width, quitButton.height };

}

void renderMenu() {
    switch(currentScene) {
        case MAIN_MENU_VIEW:
            DrawTextureRec(logo, sourceLogoRec, (Vector2){logoBounds.x, logoBounds.y}, RAYWHITE);
            DrawTextureRec(playButton, sourcePlayBtnRec, (Vector2){ playBtnBounds.x, playBtnBounds.y }, RAYWHITE);
            DrawTextureRec(creditButton, sourceCreditBtnRec, (Vector2){ creditBtnBounds.x, creditBtnBounds.y}, RAYWHITE);
            DrawTextureRec(quitButton, sourceQuitBtnRec, (Vector2){quitBtnBounds.x, quitBtnBounds.y}, RAYWHITE);
            break;
        case SAVE_MENU_VIEW:
            DrawTexturePro(saveMenuBackground, (Rectangle){0, 0, (float)saveMenuBackground.width, (float)saveMenuBackground.height}, (Rectangle){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, (Vector2){0, 0}, 0, RAYWHITE);
            DrawTexturePro(newGameButton, (Rectangle){0, 0, (float)newGameButton.width, (float)newGameButton.height}, (Rectangle){WINDOW_WIDTH * 0.05, WINDOW_HEIGHT * 0.87, (float)newGameButton.width, (float)newGameButton.height}, (Vector2){0, 0}, 0, RAYWHITE);
            break;
        default:
            break;
    }
}

void MenuLoop() {
    renderMenu();
    handleMenu();
}

void handleMainMenu() {
	//creditBtnAction = false;
	isInMenu = true;

	mousePoint = (Vector2){ 0.0f, 0.0f };

	mousePoint = GetMousePosition();
	if(CheckCollisionPointRec(mousePoint, playBtnBounds)) {
		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentScene = SAVE_MENU_VIEW;
        }

	}

	if(CheckCollisionPointRec(mousePoint, quitBtnBounds)) {
		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			 quitBtnAction = true;
		}
	}

	if(CheckCollisionPointRec(mousePoint, creditBtnBounds)) {
		//if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) creditBtnAction = true;
	}
}

void handleSaveMenu() {
    mousePoint = (Vector2){ 0.0f, 0.0f };

    mousePoint = GetMousePosition();
    if(CheckCollisionPointRec(mousePoint, newGameButtonBounds)) {
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            playBtnAction = true;
        }
    }
}

void handleMenu() {
    switch (currentScene) {
        case MAIN_MENU_VIEW:
            handleMainMenu();
            break;
        case SAVE_MENU_VIEW:
            handleSaveMenu();
            break;
        default:
            break;
    }
}

Scenes_e getCurrentScene() {
	return currentScene;
}

bool getQuitGameAction() {
	return quitBtnAction;
}

bool getPlayGameAction() {
	return playBtnAction;
}


void destroyMenu() {
    UnloadTexture(playButton);
    UnloadTexture(creditButton);
    UnloadTexture(quitButton);
    UnloadTexture(logo);
}

