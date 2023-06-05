#include "../../includes/raylib.h"
#include <stdlib.h>
#include <stdio.h>
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

float playButtonFrame;
float creditButtonFrame;
float quitButtonFrame;
float logoFrame;

Rectangle playBtnBounds;
Rectangle creditBtnBounds;
Rectangle quitBtnBounds;
Rectangle logoBounds;

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

	playBtnBounds = (Rectangle){ SCREEN_WIDTH/2.0f - playButton.width/2.0f, 400, (float)playButton.width, playButtonFrame };
	creditBtnBounds = (Rectangle){ SCREEN_WIDTH/2.0f - creditButton.width/2.0f, 520 , (float)creditButton.width, creditButtonFrame };
	quitBtnBounds = (Rectangle){ SCREEN_WIDTH/2.0f - quitButton.width/2.0f, 640, (float)quitButton.width, quitButtonFrame };
	logoBounds = (Rectangle){SCREEN_WIDTH/50.0f - logo.width/10.0f,-360, (float)logo.width, logoFrame};

	sourceLogoRec = (Rectangle){0, 0, (float)logo.width, logoFrame};
	sourcePlayBtnRec = (Rectangle){ 0, 0, (float)playButton.width, playButtonFrame };
	sourceCreditBtnRec = (Rectangle){ 0, 0, (float)creditButton.width, creditButtonFrame };
	sourceQuitBtnRec = (Rectangle){ 0, 0, (float)quitButton.width, quitButtonFrame };

	playButtonFrame = (float)playButton.height;
	creditButtonFrame = (float)creditButton.height;
	quitButtonFrame = (float)quitButton.height;
	logoFrame = (float)logo.height;
}

void renderMainMenu() {
	initMenu();

	DrawTextureRec(logo, sourceLogoRec, (Vector2){logoBounds.x, logoBounds.y}, RAYWHITE);
	DrawTextureRec(playButton, sourcePlayBtnRec, (Vector2){ playBtnBounds.x, playBtnBounds.y }, RAYWHITE);
	DrawTextureRec(creditButton, sourceCreditBtnRec, (Vector2){ creditBtnBounds.x, creditBtnBounds.y}, RAYWHITE);
	DrawTextureRec(quitButton, sourceQuitBtnRec, (Vector2){quitBtnBounds.x, quitBtnBounds.y}, RAYWHITE);
}

void handleMenu() {
	//creditBtnAction = false;
	isInMenu = true;

	mousePoint = (Vector2){ 0.0f, 0.0f };

	mousePoint = GetMousePosition();
	if(CheckCollisionPointRec(mousePoint, playBtnBounds)) {
		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			playBtnAction = true;
		};
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

Scenes_e getCurrentScene() {
	return currentScene;
}

bool getQuitGameAction() {
	return quitBtnAction;
}

bool getPlayGameAction() {
	return playBtnAction;
}