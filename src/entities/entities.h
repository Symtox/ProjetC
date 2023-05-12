typedef struct position_t {
    int x;
    int y;
} position_t;

typedef struct player_t
{
    int max_hp;
    int hp;
    int defense;
    int attack;
    int nbKey;
    position_t pos;
} player_t;

typedef struct monster_t
{
    int max_hp;
    int hp;
    int defense;
    int attack;
} monster_t;