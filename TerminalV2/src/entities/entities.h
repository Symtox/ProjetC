#ifndef MAX_SIZE

    #define MAX_SIZE 30
    #define MAX_ROOMS 4

    typedef struct position_t
    {
        int x;
        int y;
    }position_t;

    typedef struct player_t 
    {
        int max_hp;
        int hp;
        int defense;
        int attack;
        int nbKey;
        position_t pos;
    }player_t;

    typedef struct monsterClass_t
    {
        char name;
        int max_hp;
        int defense;
        int attack;
    }monsterClass_t;

    typedef struct monster_t
    {
        char name;
        int max_hp;
        int hp;
        int defense;
        int attack;
        position_t pos;
    }monster_t;

    typedef struct map_t {
        char * name;
        char table[MAX_SIZE][MAX_SIZE];
        int monsterCount;
        int monsterClassCount;
        monsterClass_t * monsterClass;
        char * east;
        char * west;
        char * north;
        char * south;
        int generatedNumber;
    }map_t;

    typedef struct tabMonsters_t
    {
        char * levelName;
        monster_t * monsters;
        int nbMonsters;
    }tabMonsters_t;

#endif