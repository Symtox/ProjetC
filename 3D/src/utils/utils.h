#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
typedef struct {
    Vector3 position;
    Vector3 direction;
    int timeToLive;
}bullet_t;
#define BULLET_SPEED 0.9f

int logFile(const char *);
float distance3D(Vector3, Vector3);
void initLogger();
void endLogger();
void benchmarkStart(int index, char * msg);
void benchmarkEnd(int index);
#define MAX2(a,b) ((a>b)?(a):(b))
#define MAX4(a,b,c,d) MAX2(MAX2(a,b), MAX2(c,d))
#define MIN2(a,b) ((a<b)?(a):(b))
#define MIN4(a,b,c,d) MIN2(MIN2(a,b), MIN2(c,d))
#endif
