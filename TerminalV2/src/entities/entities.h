#define MAX_SIZE 30


typedef struct player_t 
{
    int max_hp;
    int hp;
    int defense;
    int attack;
    int nbKey;
}player_t;

typedef struct monster_t
{
    char name;
    int max_hp;
    int hp;
    int defense;
    int attack;
}monster_t;

typedef struct table_t{
    char plateau[MAX_SIZE][MAX_SIZE];
}table_t;


typedef struct map_t{
    table_t table;

    int monsterCount;
    int monsterClassCount;
    monster_t * monsters;
    monster_t * monsterClass;
    
    char * actual;
    char * est;
    char * ouest;
    char * nord;
    char * sud;
}map_t;