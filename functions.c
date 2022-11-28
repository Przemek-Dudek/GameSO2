#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <err.h>

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

        case '1':
            attrset(COLOR_PAIR(7));
            move(i,j);

            addch('1');
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

    display_stats(server);

    print_char('1', server->player->pos->y, server->player->pos->x);
    print_char('*', server->beast->pos->y, server->beast->pos->x);

    move(MAP_HEIGHT-1, MAP_WIDTH-1);
    refresh();
}

void free_map(char **map)
{
    if(map == NULL) return;
    for(int i = 0; *(map+i) != NULL; i++) free(*(map+i));

    free(map);
}

int read_map(char ***map)
{
    if(map == NULL) return 1;

    FILE *f = fopen(filename, "r");
    if(f == NULL)
        return 2;

    *map = calloc(MAP_HEIGHT+1, sizeof(char*));
    if(*map == NULL) {
        return 3;
    }

    for(int i = 0; i < MAP_HEIGHT; i++) {
        *(*map+i) = calloc((MAP_WIDTH+1)+1, sizeof(char));
        if(*(*map+i) == NULL) {
            free_map(*map);
            return 3;
        }
    }

    for(int i = 0; i < MAP_HEIGHT; i++) {
        if(fgets(*(*map+i), (MAP_WIDTH+1)+1, f) == NULL) break;
        *(*(*map+i)+(MAP_WIDTH+1)-1) = '\0';
    }

    fclose(f);

    return 0;
}

struct pos *move_check(int input, struct player *player, char **map)
{
    struct pos *position = player->pos;
    switch (input) {
        case 'w': case 'W':
            if(position->y == 0 || *(*(map+2-1)+2) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2-1)+2) == '#') player->krzok = 1;


            position->y -= 1;
            return position;

        case 'a': case 'A':
            if(position->x == 0 || *(*(map+2)+2-1) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2)+2-1) == '#') player->krzok = 1;

            position->x -= 1;
            return position;

        case 's': case 'S':
            if(position->y == MAP_HEIGHT-1 || *(*(map+2+1)+2) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2+1)+2) == '#') player->krzok = 1;


            position->y += 1;
            return position;

        case 'd': case 'D':
            if(position->x == (MAP_WIDTH+1)-1 || *(*(map+2)+2+1) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2)+2+1) == '#') player->krzok = 1;

            position->x += 1;
            return position;

        default:
            return position;
    }
}


int prepServer(char ***map, struct server **server) {
    *server = calloc(1, sizeof(struct server));
    (*server)->player = calloc(1, sizeof(struct player));
    (*server)->player->pos = calloc(1, sizeof(struct pos));

    (*server)->beast = calloc(1, sizeof(struct beast));
    (*server)->beast->pos = calloc(1, sizeof(struct pos));

    (*server)->player->view = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *((*server)->player->view+i) = calloc(6, sizeof(char));
    }

    (*server)->beast->view = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *((*server)->beast->view+i) = calloc(6, sizeof(char));
    }

    map_gen();

    int err = read_map(map);

    if(err == 2) {
        printw("Cold not open file \"%s\"", filename);
        refresh();
        getch();
        endwin();
        return 1;
    }

    for(int i = 0; i < MAP_HEIGHT; i++) {
        for(int j = 0; j < MAP_WIDTH; j++) {
            if(*(*(*map+i)+j) == 'A') {
                (*server)->campsite.x = j;
                (*server)->campsite.y = i;
            }
        }
    }

    (*server)->player->pos = find_avb_pos(*map, (*server)->player->pos);
    (*server)->beast->pos = find_avb_pos(*map, (*server)->beast->pos);

    return 0;
}

void *player_handle(void *arg)
{
    struct player *player = (struct player*) arg;
    player->krzok = 0;

    while(1) {
        pthread_mutex_lock(&player->player_m);
        if(quitFlag == 'q' || quitFlag == 'Q') {
            break;
        }

        flushinp();

        timeout(500);

        quitFlag = getchar();
        timeout(-1);

        //if(quitFlag == err) quitFlag = '0';

        player->pos = move_check(quitFlag, player, player->view);
    }
}

void *beast_handle(void *arg)
{
    struct server *server = (struct server*) arg;

    while(1) {
        pthread_mutex_lock(&server->beast->beast_m);

        if(quitFlag == 'q' || quitFlag == 'Q') {
            break;
        }
    }
}

void all_players_matter(struct player *player, char** map)
{
    char curr = *(*(map+player->pos->y)+player->pos->x);

    switch (curr) {
        case 'A':
            player->bank += player->carried;
            player->carried = 0;
            break;

        case 'c':
            *(*(map+player->pos->y)+player->pos->x) = ' ';
            player->carried += COIN_VALUE;
            break;

        case 't':
            *(*(map+player->pos->y)+player->pos->x) = ' ';
            player->carried += TREASURE_VALUE;
            break;

        case 'T':
            *(*(map+player->pos->y)+player->pos->x) = ' ';
            player->carried += LARGE_TREASURE_VALUE;
            break;

        case '#':
            break;

        default:
            *(*(map+player->pos->y)+player->pos->x) = ' ';
            break;
    }
}

char** player_vision(struct server *server, char **map) {
    char **vision = server->player->view;

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            *(*(vision+i)+j) = 'B';

            if(server->player->pos->y-2+i >= 0 && server->player->pos->y-1+i <= MAP_HEIGHT &&
                    server->player->pos->x-2+j >= 0 && server->player->pos->x-1+j <= MAP_WIDTH) {
                *(*(vision+i)+j) = *(*(map+server->player->pos->y-2+i)+server->player->pos->x-2+j);
            }
        }
    }

    return vision;
}

char** beast_vision(struct server *server, char **map) {
    char **vision = server->beast->view;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            *(*(vision+i)+j) = 'B';

            if(server->beast->pos->y-2+i >= 0 && server->beast->pos->y-1+i <= MAP_HEIGHT &&
               server->beast->pos->x-2+j >= 0 && server->beast->pos->x-1+j <= MAP_WIDTH) {
                *(*(vision+i)+j) = *(*(map+server->beast->pos->y-2+i)+server->beast->pos->x-2+j);
            }
        }
    }

    return vision;
}
