#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include "config.h"

#include "functions.h"

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

void display_stats(struct server *server)
{
    printw("Campsite X/Y %d/%d", server->campsite.x, server->campsite.y);
    move(3, (MAP_WIDTH+1)+1);
    printw("Player 1");
    move(4, (MAP_WIDTH+1)+1);
    printw("Position X/Y %d/%d  ", server->player->pos->x, server->player->pos->y);
    move(5, (MAP_WIDTH+1)+1);
    printw("Coins carried - %d  ", server->player->carried);
    move(6, (MAP_WIDTH+1)+1);
    printw("Coins brought - %d  ", server->player->bank);
    move(7, (MAP_WIDTH+1)+1);
    printw("Round = %d", server->round);
    move(8, (MAP_WIDTH+1)+1);
    printw("Beast position X/Y %d/%d  ", server->beast->pos->x, server->beast->pos->y);
    move(MAP_HEIGHT+1, 1);
    printw("Q/q to quit");
}

void display_map(char **map, struct server *server)
{
    if(map == NULL) return;

    for(int i = 0; *(map+i) != NULL; i++) {
        for(int j = 0; *(*(map+i)+j) != '\0'; j++)
        {
            print_char(*(*(map+i)+j), i, j);
        }
    }

    move(1, (MAP_WIDTH+1)+1);

    if (server != NULL) {
        display_stats(server);
        print_char('1', server->player->pos->y, server->player->pos->x);
        print_char('*', server->beast->pos->y, server->beast->pos->x);
    } else {
        print_char('2', 2, 2);
    }

    if(server != NULL) {
        move(MAP_HEIGHT-1, MAP_WIDTH-1);
    } else {
        move(10, 10);
    }

    refresh();
}

void free_map(char **map)
{
    if(map == NULL) return;
    for(int i = 0; *(map+i) != NULL; i++) free(*(map+i));

    free(map);
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

    for (int k = 0; k < 1000; ++k) {

        for(int i = 0; i < 5; i++) {
            for(int j = 0; j < 5; j++) {
                char tmp;
                read(w_fifo, &tmp, sizeof(char));
                *(*(map+i)+j) = tmp;
            }
        }



        display_map(map, NULL);
    }

    close(r_fifo);
    close(w_fifo);

    endwin();

    return 0;
}