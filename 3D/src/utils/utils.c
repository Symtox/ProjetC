#include "../../includes/raylib.h"
#include "./utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAX2(a,b) ((a>b)?(a):(b))
#define MAX4(a,b,c,d) MAX2(MAX2(a,b), MAX2(c,d))

FILE * fp;


/**
 * Logs to the log file and to the console
 */
int logFile(const char * msg) {
    printf("%s\n", msg);
    fflush(stdout);
    fwrite(msg, sizeof(char), strlen(msg), fp);
    fwrite("\n", sizeof(char), 1, fp);
    return 0;
}

/**
 * Open the log file
 */
void initLogger() {
    fp = fopen("log.txt", "w");
    if(fp == NULL) {
        printf("Can't log to file\n");
    }
}

/**
 * Calculate the distance between two points
 * @return
 */
float distance3D(Vector3 v1, Vector3 v2) {
    return sqrtf(powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2) + powf(v1.z - v2.z, 2));
}

/**
 * close the file
 */
void endLogger() {
    fclose(fp);
}

