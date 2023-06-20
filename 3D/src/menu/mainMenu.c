#include "../../includes/raylib.h"
#include "mainMenu.h"
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#define MAX_SAVE_COUNT 6
//////////////Varibales
bool quitBtnAction = false;
bool playBtnAction = false;
bool isInMenu;


Texture2D playButton;
Texture2D quitButton;
Texture2D creditButton;
Texture2D backButton;
Texture2D logo;

Texture2D saveMenuBackground;
Texture2D saveButtonBackground;
Texture2D newGameButton;

Rectangle playBtnBounds;
Rectangle creditBtnBounds;
Rectangle quitBtnBounds;
Rectangle backBtnBounds;
Rectangle logoBounds;
Rectangle newGameButtonBounds;
Rectangle firstSaveBounds;

Rectangle sourceLogoRec;
Rectangle sourcePlayBtnRec;
Rectangle sourceCreditBtnRec;
Rectangle sourceBackBtnRec;
Rectangle sourceQuitBtnRec;

Vector2 mousePoint;

int saveCount = 0;

Scenes_e currentScene = MAIN_MENU_VIEW;
char * saveName = NULL;
char saves[MAX_SAVE_COUNT][100];
//////////////

void initMenu() {
	playButton = LoadTexture("assets/menu/PlayButton.png");
	creditButton = LoadTexture("assets/menu/CreditButton.png");
	quitButton = LoadTexture("assets/menu/QuitButton.png");
	backButton = LoadTexture("assets/menu/backButton.png");
	logo = LoadTexture("assets/menu/mazeSlayerLogo.png");
    saveMenuBackground = LoadTexture("assets/menu/SaveMenu.png");
    saveButtonBackground = LoadTexture("assets/menu/saveButtonBackground.png");
    newGameButton = LoadTexture("assets/menu/newGameBtn.png");



    playBtnBounds = (Rectangle){ GetScreenWidth()/2.0f - playButton.width/2.0f, 500, (float)playButton.width, playButton.height };
    creditBtnBounds = (Rectangle){ GetScreenWidth()/2.0f - creditButton.width/2.0f, 650 , (float)creditButton.width, creditButton.height };
    quitBtnBounds = (Rectangle){ GetScreenWidth()/2.0f - quitButton.width/2.0f, 800, (float)quitButton.width, quitButton.height };
    backBtnBounds = (Rectangle){ GetScreenWidth() * 0.85 , GetScreenHeight() * 0.020, (float)backButton.width, backButton.height };
    logoBounds = (Rectangle){GetScreenWidth()/2 - logo.width/2,-360, (float)logo.width, logo.height};
    firstSaveBounds = (Rectangle){GetScreenWidth() * 0.05, GetScreenHeight() * 0.87, (float)newGameButton.width, (float)newGameButton.height};

    newGameButtonBounds = (Rectangle){ GetScreenWidth() * 0.05, GetScreenHeight() * 0.87, (float)newGameButton.width, newGameButton.height };

    sourceLogoRec = (Rectangle){0, 0, logo.width, logo.height};
    sourcePlayBtnRec = (Rectangle){ 0, 0, (float)playButton.width, playButton.height };
    sourceCreditBtnRec = (Rectangle){ 0, 0, (float)creditButton.width, creditButton.height };
    sourceQuitBtnRec = (Rectangle){ 0, 0, (float)quitButton.width, quitButton.height };
    sourceBackBtnRec = (Rectangle) {0, 0, (float)backButton.width, backButton.height };

}

void renderMenu() {
    switch(currentScene) {
        case MAIN_MENU_VIEW:
        	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BROWN);
            DrawTextureRec(logo, sourceLogoRec, (Vector2){logoBounds.x, logoBounds.y}, RAYWHITE);
            DrawTextureRec(playButton, sourcePlayBtnRec, (Vector2){ playBtnBounds.x, playBtnBounds.y }, RAYWHITE);
            DrawTextureRec(creditButton, sourceCreditBtnRec, (Vector2){ creditBtnBounds.x, creditBtnBounds.y}, RAYWHITE);
            DrawTextureRec(quitButton, sourceQuitBtnRec, (Vector2){quitBtnBounds.x, quitBtnBounds.y}, RAYWHITE);
            break;
        case SAVE_MENU_VIEW:
            DrawTexturePro(saveMenuBackground, (Rectangle){0, 0, (float)saveMenuBackground.width, (float)saveMenuBackground.height}, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, (Vector2){0, 0}, 0, RAYWHITE);
            DrawTexturePro(newGameButton, (Rectangle){0, 0, (float)newGameButton.width, (float)newGameButton.height}, (Rectangle){GetScreenWidth() * 0.05, GetScreenHeight() * 0.87, (float)newGameButton.width, (float)newGameButton.height}, (Vector2){0, 0}, 0, RAYWHITE);
            DrawTextureRec(backButton, sourceBackBtnRec, (Vector2){backBtnBounds.x, backBtnBounds.y}, RAYWHITE);
            for(int i = 0; i < saveCount; i++) {
                if(saves[i] != NULL) {
                    DrawTexturePro(saveButtonBackground, (Rectangle){0, 0, (float)saveButtonBackground.width, (float)saveButtonBackground.height}, (Rectangle){GetScreenWidth() * 0.05, GetScreenHeight() * 0.2 + (i * 125), (float)saveButtonBackground.width, (float)saveButtonBackground.height}, (Vector2){0, 0}, 0, RAYWHITE);
                    DrawText(saves[i], GetScreenWidth() * 0.1, GetScreenHeight() * 0.25 + (i * 125), 20, RAYWHITE);
                }
            }
            break;
        case CREDIT_VIEW:
        	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BROWN);
        	DrawTextureRec(backButton, sourceBackBtnRec, (Vector2){backBtnBounds.x, backBtnBounds.y}, RAYWHITE);
            DrawText("Developement :", 50, 50, 30, CLITERAL(Color){255, 255, 255, 255});
            DrawText("Maxime Millat", 300, 50, 30, CLITERAL(Color){200, 200, 200, 200});
            DrawText("Enzo Dupré", 300, 100, 30, CLITERAL(Color){200, 200, 200, 200});
            DrawText("Théo Mouisse", 300, 150, 30, CLITERAL(Color){200, 200, 200, 200});
            DrawText("Clément Mabile", 300, 200, 30, CLITERAL(Color){200, 200, 200, 200});

            DrawText("Textures :", 50, 300, 30, CLITERAL(Color){255, 255, 255, 255});
            DrawText("Clément Mabile", 300, 300, 30, CLITERAL(Color){200, 200, 200, 200});
            DrawText("Enzo Dupré", 300, 350, 30, CLITERAL(Color){200, 200, 200, 200});

            DrawText("Modeling :", 50, 450, 30, CLITERAL(Color){255, 255, 255, 255});
            DrawText("Clément Mabile", 300, 450, 30, CLITERAL(Color){200, 200, 200, 200});

            DrawText("Sources :", 50, 550, 30, CLITERAL(Color){255, 255, 255, 255});
            DrawText("https://www.raylib.com/examples.html", 300, 550, 30, CLITERAL(Color){200, 200, 200, 200});
            DrawText("https://discord.com/invite/raylib", 300, 600, 30, CLITERAL(Color){200, 200, 200, 200});

            DrawText("Dernière modification: 19/06/2023", 1500, 1100, 20, CLITERAL(Color){200, 200, 200, 200});
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
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScene = SAVE_MENU_VIEW;
            getAllSaves();
        }

	}

	if(CheckCollisionPointRec(mousePoint, quitBtnBounds)) {
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			 quitBtnAction = true;
		}
	}

	if(CheckCollisionPointRec(mousePoint, creditBtnBounds)) {
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             currentScene = CREDIT_VIEW;
        }
	}

}

void handleSaveMenu() {
    mousePoint = (Vector2){ 0.0f, 0.0f };

    mousePoint = GetMousePosition();
    if(CheckCollisionPointRec(mousePoint, newGameButtonBounds)) {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            playBtnAction = true;
        }
    }

    if(CheckCollisionPointRec(mousePoint, backBtnBounds)) {
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			currentScene = MAIN_MENU_VIEW;
		}
	}

    for(int i = 0; i < saveCount; i++) {
        if(saves[i] != NULL) {
            if(CheckCollisionPointRec(mousePoint, (Rectangle){GetScreenWidth() * 0.05, GetScreenHeight() * 0.2 + (i * 125), (float)saveButtonBackground.width, (float)saveButtonBackground.height})) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    saveName = malloc(sizeof(char) * strlen(saves[i]));
                    strcpy(saveName, saves[i]);
                    playBtnAction = true;
                }
            }
        }
    }

}

void handleCreditView() {
	mousePoint = (Vector2){ 0.0f, 0.0f };

	mousePoint = GetMousePosition();

	if(CheckCollisionPointRec(mousePoint, backBtnBounds)) {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentScene = MAIN_MENU_VIEW;
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
        case CREDIT_VIEW:
        	handleCreditView();
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

char * getSaveName() {
    return saveName;
}

void setCurrentScene(Scenes_e scene) {
    saveName = NULL;
    playBtnAction = false;
    currentScene = scene;
    EnableCursor();
}

void destroyMenu() {
    UnloadTexture(playButton);
    UnloadTexture(creditButton);
    UnloadTexture(quitButton);
    UnloadTexture(logo);
}

void getAllSaves() {
    DIR *d;
    saveCount = 0;
    struct dirent *dir;
    d = opendir("./bin/saves/");
    if (d) {
        while ((dir = readdir(d)) != NULL && saveCount < MAX_SAVE_COUNT) {
            if(strcmp(".", dir->d_name) != 0 && strcmp("..", dir->d_name) != 0) {
                strcpy(saves[saveCount], dir->d_name);
                saveCount++;
            }
        }
        closedir(d);
    }
}

