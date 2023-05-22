//
// Created by 20015007@hq.corp.leroymerlin.com on 13/05/23.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
int fillWithRandAndWalls(int ***);
int logFile(const char *);
void startLogger();
void endLogger();
#define MAX2(a,b) ((a>b)?(a):(b))
#define MAX4(a,b,c,d) MAX2(MAX2(a,b), MAX2(c,d))
#define MIN2(a,b) ((a<b)?(a):(b))
#endif //PROJECT_UTILS_H
