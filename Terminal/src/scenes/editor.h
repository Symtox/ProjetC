#include "../components/saveSelector.h"
#ifndef PROJECT_EDITOR
#define PROJECT_EDITOR

/*
 * Structure de données pour le menu
 */
typedef struct {
    WINDOW * boxwin, * win;
    SaveSelector_c saveSelector;
}EditorScene;

int newEditorScene(EditorScene *);
int dispatchEditorClick(MEVENT, EditorScene *);
int destroyEditorScene(EditorScene);
FILE** getAllSaves();

#endif
