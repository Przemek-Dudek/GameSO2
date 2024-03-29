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
int quitFlag;

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
    init_pair(9, COLOR_YELLOW, COLOR_YELLOW);

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

            case 'T': case 'D':
            attrset(COLOR_PAIR(6));
            move(i,j);

            addch(ch);
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
            attrset(COLOR_PAIR(9));
            move(i,j);

            addch(' ');
            attroff(COLOR_PAIR(9));
            break;

        default:
            attrset(COLOR_PAIR(8));
            move(i,j);

            addch('?');
            attroff(COLOR_PAIR(8));
            break;
    }
}

void display_stats(struct server *server)
{
    move(0, 8);
    printw("Server PID: %d", server->pId);
    move(2, 8);
    printw("Player's round %d", howmany);

    move(3, 8);
    printw("Player Coord's x/y: %d/%d", server->player[1].pos->x, server->player[1].pos->y);
    move(4, 8);
    printw("Coins carried - %d  ", server->player[1].carried);
    move(5, 8);
    printw("Coins brought - %d  ", server->player[1].bank);

    if(server->campsite.y != -1) {
        move(6, 8);
        printw("Campsite x/y: %d/%d", server->campsite.x, server->campsite.y);
    } else {
        move(6, 8);
        printw("Campsite x/y: ?/?");
    }

    move(8, 1);
    printw("Q/q to quit");
}

int check_coords(struct server *server, struct pos *pos)
{
    struct pos *p = server->player[1].pos;
    return (pos->y >= p->y-2 && pos->y <= p->y+2) && (pos->x >= p->x-2 && pos->x <= p->x+2);
}

void display_map(char **map, struct server *server)
{
    if(map == NULL) return;

    for(int i = 0; i < 7; i++) {
        for(int j = 0; j < 7; j++) {
            if(i == 0 || j == 0 || i == 6 || j == 6) {
                print_char('B', i, j);
            }

            if(i < 5 && j < 5) {
                print_char(*(*(map+i)+j), i+1, j+1);
                if(server->campsite.y == -1) {
                    if (*(*(map + i) + j) == 'A') {
                        server->campsite.x = server->player[1].pos->x + j-2;
                        server->campsite.y = server->player[1].pos->y + i-2;
                    }
                }
            }
        }
    }

    move(1, (MAP_WIDTH+1)+1);

    print_char('2', 3, 3);

    display_stats(server);

    if(check_coords(server, server->player->pos)) {
        print_char('1', server->player->pos->y - server->player[1].pos->y + 3, server->player->pos->x - server->player[1].pos->x + 3);
    }

    if(check_coords(server, server->beast->pos)) {
        print_char('*', server->beast->pos->y - server->player[1].pos->y + 3, server->beast->pos->x - server->player[1].pos->x + 3);
    }

    for(int i = 0; i < 10; i++) {
        if(server->graves[i].pos.y != -1 && check_coords(server, &server->graves[i].pos)) {
            print_char('D', server->graves[i].pos.y - server->player[1].pos->y + 2, server->graves[i].pos.x - server->player[1].pos->x + 2);
        }
    }

    move(10, 10);

    refresh();
}

void free_map(char **map)
{
    if(map == NULL) return;
    for(int i = 0; *(map+i) != NULL; i++) free(*(map+i));

    free(map);
}

int input;

void *input_handle(void *arg)
{
    struct player *player = (struct player*)arg;

    while(1) {
        pthread_mutex_lock(&player->player_m);

        flushinp();

        input = getch();
        if(input == 'q' || input == 'Q' || quitFlag == 1) {
            break;
        }
    }
}

int main()
{
    initscr();
    start_color();

    printw("Oczekiwanie na Server");
    refresh();

    keypad(stdscr, true);

    int r_fifo = open("r_player", O_WRONLY);
    int w_fifo = open("w_player", O_RDONLY);

    char **map = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *(map+i) = calloc(6, sizeof(char));
    }

    struct server *server = calloc(1, sizeof(struct server));
    server->player = calloc(2, sizeof(struct player));

    server->player[1].fifo = r_fifo;
    server->player[1].view = map;

    server->player->pos = calloc(1, sizeof(struct pos));
    server->player[1].pos = calloc(1, sizeof(struct pos));

    server->beast = calloc(1, sizeof(struct beast));
    server->beast->pos = calloc(1, sizeof(struct pos));

    server->campsite.y = -1;

    pthread_mutex_init(&server->player[1].player_m, NULL);
    pthread_create(&server->player[1].player_t, NULL, input_handle, server->player+1);

    for(howmany = 0;;howmany++) {
        for(int i = 0; i < 5; i++) {
            for(int j = 0; j < 5; j++) {
                char tmp;
                read(w_fifo, &tmp, sizeof(char));
                *(*(map+i)+j) = tmp;
            }
        }

        for(int i = 0; i < 10; i++) {
            int x, y, a;

            read(w_fifo, &x, sizeof(int));
            read(w_fifo, &y, sizeof(int));
            read(w_fifo, &a, sizeof(int));

            server->graves[i].pos.x = x;
            server->graves[i].pos.y = y;
            server->graves[i].amount = x;
        }

        int tmp;
        read(w_fifo, &tmp, sizeof(int));
        server->player->pos->x = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->player->pos->y = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->player[1].pos->x = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->player[1].pos->y = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->player[1].bank = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->player[1].carried = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->pId = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->beast->pos->x = tmp;

        read(w_fifo, &tmp, sizeof(int));
        server->beast->pos->y = tmp;

        read(w_fifo, &quitFlag, sizeof(int));

        display_map(map, server);

        server->player[1].view = map;

        pthread_mutex_unlock(&server->player[1].player_m);

        write(server->player[1].fifo, &input, sizeof(int));

        if(input == 'q' || input == 'Q' || quitFlag == 1) {
            break;
        }

        input = '0';
    }

    close(r_fifo);
    close(w_fifo);

    endwin();

    return 0;
}