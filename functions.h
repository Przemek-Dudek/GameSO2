#ifndef FUNCTIONS
#define FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int quitFlag = 0;
char p2In;
int testsik = 0;

struct pos {
    int x;
    int y;
};

struct player {
    pthread_mutex_t player_m;
    pthread_t player_t;
    struct pos *pos;

    int krzok;
    int is_there;

    char **view;

    int carried;
    int bank;
};

struct beast {
    pthread_t beast_t;
    pthread_mutex_t beast_m;

    int krzok;
    char **view;
    char dir;

    struct pos *pos;
};

struct grave {
    struct pos pos;
    int amount;
};

struct server {
    struct pos campsite;
    struct player *player;
    struct beast *beast;
    struct grave graves[10];

    int pId;

    int round;
};


void print_char(char ch, int i, int j);
void display_stats(struct server *server);
void display_map(char **map, struct server *server);
void free_map(char **map);
int read_map(char ***map);
struct pos *move_check(int input, struct player *player, char **map);
struct pos *beast_move_check(int input, struct beast *player, char **map, int *flag);

struct pos *find_avb_pos(char **map, struct pos *position);
int move_pos(struct pos *cur, char **map, int step);
void krzak_populate(char **map, int step);
void display(char **map);
int map_gen();

void *player_handle(void *arg);

void spawn_coin(char ***map);
void spawn_treasure(char ***map);
void spawn_large_treasure(char ***map);
void spawn_bestia(char ***map);

int prepServer(char ***map, struct server **server);

void send_struct(struct server *server, int w_fifo);

#endif