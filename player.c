#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "config.h"

int howmany;

struct pos {
    int x;
    int y;
};

struct beast {
    pthread_t beast_t;
    pthread_mutex_t beast_m;

    int krzok;
    char **view;

    struct pos *pos;
};

struct player {
    pthread_mutex_t player_m;
    pthread_t player_t;
    struct pos *pos;

    int krzok;
    int is_there;
    int fifo;

    char **view;

    int carried;
    int bank;
};

struct server {
    struct pos campsite;
    struct player *player;
    struct beast *beast;

    int pId;

    int round;
};

void print_char(char ch, int i, int j)
{
    init_pair(1, COLOR_BLACK, COLOR_BLACK); //PATH
    init_pair(2, COLOR_WHITE, COLOR_WHITE); //WALL
    init_pair(3, COLOR_WHITE, COLOR_BLACK); //KRZAK
    init_pair(4, COLOR_YELLOW, COLOR_GREEN); //CAMP
    init_pair(5, COLOR_GREEN, COLOR_BLACK); //COIN & SMALL TREASURE
    init_pair(6, COLOR_BLUE, COLOR_BLACK); //LARGE TREASURE
    init_pair(7, COLOR_WHITE, COLOR_BLUE); //PLAYER
    init_pair(8, COLOR_WHITE, COLOR_RED); //BESTIA

    switch (ch) {
        case 'W':
            attrset(COLOR_PAIR(2));
            move(i,j);

            addch(' ');
            attroff(COLOR_PAIR(2));
            break;

        case ' ':
            attrset(COLOR_PAIR(1));
            move(i,j);

            addch(' ');
            attroff(COLOR_PAIR(1));
            break;

        case '#':
            attrset(COLOR_PAIR(3));
            move(i,j);

            addch('#');
            attroff(COLOR_PAIR(3));
            break;

        case 'A':
            attrset(COLOR_PAIR(4));
            move(i,j);

            addch('A');
            attroff(COLOR_PAIR(4));
            break;

        case 'c':
            attrset(COLOR_PAIR(5));
            move(i,j);

            addch('c');
            attroff(COLOR_PAIR(5));
            break;

        case 'T':
            attrset(COLOR_PAIR(6));
            move(i,j);

            addch('T');
            attroff(COLOR_PAIR(6));
            break;

        case 't':
            attrset(COLOR_PAIR(5));
            move(i,j);

            addch('t');
            attroff(COLOR_PAIR(5));
            break;

            case '1': case '2':
            attrset(COLOR_PAIR(7));
            move(i,j);

            addch(ch);
            attroff(COLOR_PAIR(7));
            break;

        case '*':
            attrset(COLOR_PAIR(8));
            move(i,j);

            addch('*');
            attroff(COLOR_PAIR(8));
            break;

        case 'B':
            attrset(COLOR_PAIR(8));
            move(i,j);

            addch(' ');
            attroff(COLOR_PAIR(8));
            break;

        default:
            attrset(COLOR_PAIR(8));
            move(i,j);

            addch('?');
            attroff(COLOR_PAIR(8));
            break;
    }
}

void display_stats(struct player *player)
{
    move(3, 7);
    printw("Round %d", howmany);
    move(7, 1);
    printw("Q/q to quit");
}

void display_map(char **map, struct player *player)
{
    if(map == NULL) return;

    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            if(i < 5 && j < 5) {
                print_char(*(*(map+i)+j), i, j);
            } else {
                print_char('B', i, j);
            }
        }
    }

    move(1, (MAP_WIDTH+1)+1);

    print_char('2', 2, 2);

    display_stats(player);

    move(10, 10);

    refresh();
}

void free_map(char **map)
{
    if(map == NULL) return;
    for(int i = 0; *(map+i) != NULL; i++) free(*(map+i));

    free(map);
}

char input;

void *input_handle(void *arg)
{
    struct player *player = (struct player*)arg;

    while(1) {
        pthread_mutex_lock(&player->player_m);

        flushinp();

        input = '0';

        input = getchar();
        if(input == 'q' || input == 'Q') {
            break;
        }
    }
}

int main()
{
    initscr();
    start_color();

    int r_fifo = open("r_player", O_WRONLY);
    int w_fifo = open("w_player", O_RDONLY);

    char **map = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *(map+i) = calloc(6, sizeof(char));
    }

    struct player *player = calloc(1, sizeof(struct player));

    player->fifo = r_fifo;
    player->view = map;

    pthread_mutex_init(&player->player_m, NULL);
    pthread_create(&player->player_t, NULL, input_handle, player);

    for(howmany = 0;;howmany++) {
        for(int i = 0; i < 5; i++) {
            for(int j = 0; j < 5; j++) {
                char tmp;
                read(w_fifo, &tmp, sizeof(char));
                *(*(map+i)+j) = tmp;
            }
        }
        display_map(map, player);

        player->view = map;

        pthread_mutex_unlock(&player->player_m);

        write(player->fifo, &input, sizeof(char));

        if(input == 'q' || input == 'Q') {
            break;
        }
    }

    close(r_fifo);
    close(w_fifo);

    endwin();

    return 0;
}