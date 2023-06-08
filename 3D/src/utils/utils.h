#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
int logFile(const char *);
float distance3D(Vector3, Vector3);
void initLogger();
void endLogger();
#define MAX2(a,b) ((a>b)?(a):(b))
#define MAX4(a,b,c,d) MAX2(MAX2(a,b), MAX2(c,d))
#define MIN2(a,b) ((a<b)?(a):(b))
#define MIN4(a,b,c,d) MIN2(MIN2(a,b), MIN2(c,d))
#endif
