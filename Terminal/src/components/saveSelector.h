#ifndef PROJECT_SAVESELECTOR_H
#define PROJECT_SAVESELECTOR_H

    typedef struct {
        int noFiles;
        WINDOW * win;
        struct dirent ** files;
    }SaveSelector_c;

    int newSaveSelector(SaveSelector_c*, int, int, int);
    int filterSaveFiles(const struct dirent *);





#endif
