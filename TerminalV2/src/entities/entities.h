#ifndef MAX_SIZE

    #define MAX_SIZE 30
    #define MAX_ROOMS 6

    /*Structure position 2D*/
    typedef struct position_t
    {
        int x;
        int y;
    }position_t;

    /*Structure joueur*/
    typedef struct player_t 
    {
        int max_hp;
        int hp;
        int defense;
        int attack;
        int nbKey;
        position_t pos;
    }player_t;

    /*Structure classe de monstre*/
    typedef struct monsterClass_t
    {
        char name;
        int max_hp;
        int defense;
        int attack;
    }monsterClass_t;

    /*Structure monstre*/
    typedef struct monster_t
    {
        char name;
        int max_hp;
        int hp;
        int defense;
        int attack;
        position_t pos;
    }monster_t;

    /*Structure plateau de jeu*/
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

    /*Structure tableau de maps générés*/
    typedef struct tabMaps_t {
        map_t * maps;
        int nbMaps;
    }tabMaps_t;

    /*Structure tableau de monstre par niveau*/
    typedef struct tabMonsters_t
    {
        char * levelName;
        monster_t * monsters;
        int nbMonsters;
    }tabMonsters_t;

#endif