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
    printw("Server PID: %d",server->pId);
    move(2, (MAP_WIDTH+1)+1);
    printw("Campsite X/Y %d/%d", server->campsite.x, server->campsite.y);
    move(4, (MAP_WIDTH+1)+1);
    printw("Player 1");
    move(5, (MAP_WIDTH+1)+1);
    printw("Position X/Y %d/%d  ", server->player->pos->x, server->player->pos->y);
    move(6, (MAP_WIDTH+1)+1);
    printw("Coins carried - %d  ", server->player->carried);
    move(7, (MAP_WIDTH+1)+1);
    printw("Coins brought - %d  ", server->player->bank);
    move(8, (MAP_WIDTH+1)+1);

    if((server->player+1)->is_there == 1) {
        move(9, (MAP_WIDTH+1)+1);
        printw("Player 2");
        move(10, (MAP_WIDTH+1)+1);
        printw("Position X/Y %d/%d  ", server->player[1].pos->x, server->player[1].pos->y);
        move(11, (MAP_WIDTH+1)+1);
        printw("Coins carried - %d  ", server->player[1].carried);
        move(12, (MAP_WIDTH+1)+1);
        printw("Coins brought - %d  ", server->player[1].bank);
    }

    move(14, (MAP_WIDTH+1)+1);
    printw("Round = %d Player 2's Round = %d", server->round, testsik);
    move(15, (MAP_WIDTH+1)+1);
    printw("Beast position X/Y %d/%d    ", server->beast->pos->x, server->beast->pos->y);
    move(16, (MAP_WIDTH+1)+1);
    printw("Beast direction, state: %c, %d, %d", server->beast->dir, server->beast->state, server->beast->test);
    move(MAP_HEIGHT+1, 1);
    printw("Q/q to quit, %d     ", p2In);

    refresh();
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
        if((server->player+1)->is_there == 1) {
            print_char('2', (server->player+1)->pos->y, (server->player+1)->pos->x);
        }
        print_char('1', server->player->pos->y, server->player->pos->x);
    } else {
        print_char('1', 2, 2);
    }

    for(int i = 0; i < 10; i++) {
        if(server->graves[i].pos.y != -1) {
            print_char('D', server->graves[i].pos.y, server->graves[i].pos.x);
        }
    }

    print_char('*', server->beast->pos->y, server->beast->pos->x);

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
        case 'w': case 'W': case 259:
            if(position->y == 0 || *(*(map+2-1)+2) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2-1)+2) == '#') player->krzok = 1;


            position->y -= 1;
            return position;

        case 'a': case 'A': case 260:
            if(position->x == 0 || *(*(map+2)+2-1) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2)+2-1) == '#') player->krzok = 1;

            position->x -= 1;
            return position;

        case 's': case 'S': case 258:
            if(position->y == MAP_HEIGHT-1 || *(*(map+2+1)+2) == 'W') return position;

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2+1)+2) == '#') player->krzok = 1;


            position->y += 1;
            return position;

        case 'd': case 'D': case 261:
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

struct pos *beast_move_check(int input, struct beast *player, char **map, int *flag)
{
    struct pos *position = player->pos;
    *flag = 1;
    switch (input) {
        case 'w': case 'W':
            if(position->y == 0 || *(*(map+2-1)+2) == 'W') {
                *flag = 0;
                return position;
            }

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2-1)+2) == '#') player->krzok = 1;


            position->y -= 1;
            return position;

        case 'a': case 'A':
            if(position->x == 0 || *(*(map+2)+2-1) == 'W') {
                *flag = 0;
                return position;
            }

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2)+2-1) == '#') player->krzok = 1;

            position->x -= 1;
            return position;

        case 's': case 'S':
            if(position->y == MAP_HEIGHT-1 || *(*(map+2+1)+2) == 'W') {
                *flag = 0;
                return position;
            }

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2+1)+2) == '#') player->krzok = 1;


            position->y += 1;
            return position;

        case 'd': case 'D':
            if(position->x == (MAP_WIDTH+1)-1 || *(*(map+2)+2+1) == 'W') {
                *flag = 0;
                return position;
            }

            if(player->krzok == 1) {
                player->krzok = 0;
                return position;
            }

            if(*(*(map+2)+2+1) == '#') player->krzok = 1;

            position->x += 1;
            return position;

        default:
            flag = 0;
            return position;
    }
}

int prepServer(char ***map, struct server **server) {
    *server = calloc(1, sizeof(struct server));
    (*server)->player = calloc(2, sizeof(struct player));
    (*server)->player->pos = calloc(1, sizeof(struct pos));
    ((*server)->player+1)->pos = calloc(1, sizeof(struct pos));

    (*server)->beast = calloc(1, sizeof(struct beast));
    (*server)->beast->pos = calloc(1, sizeof(struct pos));

    (*server)->player->view = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *((*server)->player->view+i) = calloc(6, sizeof(char));
    }

    ((*server)->player+1)->view = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *(((*server)->player+1)->view+i) = calloc(6, sizeof(char));
    }

    (*server)->beast->view = calloc(6, sizeof(char*));
    for(int i = 0; i < 5; i++) {
        *((*server)->beast->view+i) = calloc(6, sizeof(char));
    }

    ((*server)->player+1)->is_there = 0;

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
    (*server)->player[1].pos = find_avb_pos(*map, (*server)->player[1].pos);
    (*server)->beast->pos = find_avb_pos(*map, (*server)->beast->pos);
    (*server)->beast->dir = '0';
    (*server)->pId = getpid();

    for(int i = 0; i < 10; i++) {
        (*server)->graves[i].amount = 0;
        (*server)->graves[i].pos.y = -1;
        (*server)->graves[i].pos.x = -1;
    }

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
        timeout(1000);

        quitFlag = getch();
        timeout(-1);

        player->pos = move_check(quitFlag, player, player->view);
    }
}

int check_coords(struct pos *pos1, struct pos *pos_m)
{
    return (pos1->y >= pos_m->y-2 && pos1->y <= pos_m->y+2) && (pos1->x >= pos_m->x-2 && pos1->x <= pos_m->x+2);
}

void relative_pos(struct pos *pos, struct pos *pos_m, struct pos *rel) {
    rel->x = pos->x - pos_m->x;
    rel->y = pos->y - pos_m->y;
}

int path_find(struct pos *rel, char **map) {
    int offset = 2;

    while(rel->y > 0 || rel->x > 0) {
        if(abs(rel->y) > abs(rel->x)) {
            if(rel->y < 0) {
                if(*(*(map+rel->y+1+offset)+rel->x+offset) == 'W') return 0;
                rel->y++;
            } else {
                if(*(*(map+rel->y-1+offset)+rel->x+offset) == 'W') return 0;
                rel->y--;
            }
        } else {
            if(abs(rel->y) < abs(rel->x)) {
                if(rel->x < 0) {
                    if(*(*(map+rel->y+offset)+rel->x+1+offset) == 'W') return 0;
                    rel->x++;
                } else {
                    if(*(*(map+rel->y+offset)+rel->x-1+offset) == 'W') return 0;
                    rel->x--;
                }
            }

            if(abs(rel->y) == abs(rel->x)) {
                if (rel->y < 0 && rel->x < 0) {
                    if (*(*(map + rel->y + 1 + offset) + rel->x + offset) == 'W' &&
                        *(*(map + rel->y + offset) + rel->x + 1 + offset) == 'W') {
                        return 0;
                    }

                    if (*(*(map + rel->y + 1 + offset) + rel->x + offset) == 'W') {
                        rel->x++;
                    } else rel->y++;
                }

                if (rel->y < 0 && rel->x > 0) {
                    if (*(*(map + rel->y + 1 + offset) + rel->x + offset) == 'W' &&
                        *(*(map + rel->y + offset) + rel->x - 1 + offset) == 'W') {
                        return 0;
                    }

                    if (*(*(map + rel->y + 1 + offset) + rel->x + offset) == 'W') {
                        rel->x--;
                    } else rel->y++;
                }

                if (rel->y > 0 && rel->x < 0) {
                    if (*(*(map + rel->y - 1 + offset) + rel->x + offset) == 'W' &&
                        *(*(map + rel->y + offset) + rel->x + 1 + offset) == 'W') {
                        return 0;
                    }

                    if (*(*(map + rel->y - 1 + offset) + rel->x + offset) == 'W') {
                        rel->x++;
                    } else rel->y--;
                }

                if (rel->y > 0 && rel->x > 0) {
                    if (*(*(map + rel->y - 1 + offset) + rel->x + offset) == 'W' &&
                        *(*(map + rel->y + offset) + rel->x - 1 + offset) == 'W') {
                        return 0;
                    }

                    if (*(*(map + rel->y - 1 + offset) + rel->x + offset) == 'W') {
                        rel->x--;
                    } else rel->y--;
                }
            }
        }
    }

    return 1;
}

void beast_chase(struct pos *rel, char **map, char *dir, int *test)
{
    int offset = 2;

    if(abs(rel->y) > abs(rel->x)) {
        if(rel->y < 0) {
            *test = 1;
            if(*(*(map+offset-1)+offset) != 'W') {
                *dir = 'W';
                return;
            }
        } else {
            if(*(*(map+offset+1)+offset) != 'W') {
                *dir = 'S';
                return;
            }
        }
    } else {
        if(abs(rel->y) < abs(rel->x)) {
            if(rel->x < 0) {
                *test = 3;
                if(*(*(map+offset)+offset-1) != 'W') {
                    *dir = 'A';
                    return;
                }
            } else {
                *test = 4;
                if(*(*(map+offset)+offset+1) != 'W') {
                    *dir = 'D';
                    return;
                }
            }
        }

        if(abs(rel->y) == abs(rel->x)) {
            if(rel->y < 0 && rel->x < 0) {
                *test = 5;
                if(*(*(map+offset-1)+offset) != 'W') {
                    *dir = 'W';
                    return;
                }

                if(*(*(map+offset)+offset-1) != 'W') {
                    *dir = 'A';
                    return;
                }
            }

            if(rel->y < 0 && rel->x > 0) {
                *test = 6;
                if(*(*(map+offset-1)+offset) != 'W') {
                    *dir = 'W';
                    return;
                }

                if(*(*(map+offset)+offset+1) != 'W') {
                    *dir = 'D';
                    return;
                }
            }

            if(rel->y > 0 && rel->x < 0) {
                *test = 7;
                if(*(*(map+offset+1)+offset) != 'W') {
                    *dir = 'S';
                    return;
                }

                if(*(*(map+offset)+offset-1) != 'W') {
                    *dir = 'A';
                    return;
                }
            }

            if(rel->y > 0 && rel->x > 0) {
                *test = 8;
                if(*(*(map+offset+1)+offset) != 'W') {
                    *dir = 'S';
                    return;
                }

                if(*(*(map+offset)+offset+1) != 'W') {
                    *dir = 'D';
                    return;
                }
            }
        }
    }
}

void *beast_handle(void *arg)
{
    struct server *server = (struct server*) arg;

    while(1) {
        pthread_mutex_lock(&server->beast->beast_m);

        server->beast->state = 0;
        server->beast->test = 0;

        struct pos player_pos;

        player_pos.x = server->player->pos->x;
        player_pos.y = server->player->pos->y;

        if(check_coords(&player_pos, server->beast->pos)) {
            server->beast->state = 2;
            struct pos rel;
            relative_pos(&player_pos, server->beast->pos, &rel);

            if(path_find(&rel, server->beast->view)) {
                server->beast->state = 1;
            }
        }

        struct pos player2_pos;

        player2_pos.x = server->player[1].pos->x;
        player2_pos.y = server->player[1].pos->y;

        if(server->player[1].is_there && check_coords(&player2_pos, server->beast->pos)) {
            server->beast->state = 4;
            struct pos rel;
            relative_pos(&player2_pos, server->beast->pos, &rel);

            if(path_find(&rel, server->beast->view)) {
                server->beast->state = 3;
            }
        }

        if(server->beast->dir == '0') {
            server->beast->dir = rand()%4;

            switch (server->beast->dir) {
                case 0:
                    server->beast->dir = 'W';
                    break;

                case 1:
                    server->beast->dir = 'A';
                    break;

                case 2:
                    server->beast->dir = 'S';
                    break;

                case 3:
                    server->beast->dir = 'D';
                    break;
            }
        }

        int flag = 1;

        while(flag != 0 && server->beast->state != 1 && server->beast->state != 3) {
            pthread_mutex_lock(&server->beast->beast_m);

            if(check_coords(&player_pos, server->beast->pos)) {
                server->beast->state = 2;
                struct pos rel;
                relative_pos(server->player->pos, server->beast->pos, &rel);

                if(path_find(&rel, server->beast->view)) {
                    server->beast->state = 1;
                }
            }

            if(server->player[1].is_there && check_coords(&player2_pos, server->beast->pos)) {
                server->beast->state = 4;
                struct pos rel;
                relative_pos(server->player[1].pos, server->beast->pos, &rel);

                if(path_find(&rel, server->beast->view)) {
                    server->beast->state = 3;
                }
            }
            beast_move_check(server->beast->dir, server->beast, server->beast->view, &flag);
        }

        if(server->beast->state != 1 && server->beast->state != 3) {
            int losulosu = rand()%27;

            int wW, wA, wS, wD;

            switch (server->beast->dir) {
                case 'W':
                    wW = 3;
                    wA = 8;
                    wS = 8;
                    wD = 8;
                    break;

                case 'A':
                    wW = 8;
                    wA = 3;
                    wS = 8;
                    wD = 8;
                    break;

                case 'S':
                    wW = 8;
                    wA = 8;
                    wS = 3;
                    wD = 8;
                    break;

                case 'D':
                    wW = 8;
                    wA = 8;
                    wS = 8;
                    wD = 3;
                    break;
            }

            if(losulosu >= 0 && losulosu < wW) {
                server->beast->dir = 'W';
            }

            if(losulosu >= wW && losulosu < wW+wA) {
                server->beast->dir = 'A';
            }

            if(losulosu >= wW+wA && losulosu < wW+wA+wS) {
                server->beast->dir = 'S';
            }

            if(losulosu >= wW+wA+wS && losulosu < wW+wA+wS+wD) {
                server->beast->dir = 'D';
            }
        } else {
            struct pos rel;
            if(server->beast->state == 1) {
                relative_pos(server->player->pos, server->beast->pos, &rel);
                beast_chase(&rel, server->beast->view, &server->beast->dir, &server->beast->test);
                beast_move_check(server->beast->dir, server->beast, server->beast->view, &flag);
            } else {
                relative_pos(server->player[1].pos, server->beast->pos, &rel);
                beast_chase(&rel, server->beast->view, &server->beast->dir, &server->beast->test);
                beast_move_check(server->beast->dir, server->beast, server->beast->view, &flag);
            }
        }

        if(quitFlag == 'q' || quitFlag == 'Q') {
            break;
        }
    }
}

void add_grave(struct server *server, struct pos *pos, int amount)
{
    for(int i = 0; i < 10; i++) {
        if(server->graves[i].pos.y == -1) {
            server->graves[i].pos.y = pos->y;
            server->graves[i].pos.x = pos->x;

            server->graves[i].amount = amount;
            break;
        }
    }
}

void check_collision(struct server *server, char** map) {
    struct pos *p1 = server->player->pos;
    struct pos *p2 = server->player[1].pos;
    struct pos *b = server->beast->pos;

    if(p1->y == p2->y && p1->x == p2->x && server->player[1].is_there == 1) {
        int amount = 0;
        amount += server->player->carried;
        amount += server->player[1].carried;
        if(amount != 0) add_grave(server, p1, amount);

        server->player->carried = 0;
        server->player[1].carried = 0;

        server->player->pos = find_avb_pos(map, server->player->pos);
        server->player[1].pos = find_avb_pos(map, server->player[1].pos);
    }

    if(p1->y == b->y && p1->x == b->x) {
        int amount = 0;
        amount += server->player->carried;
        if(amount != 0) add_grave(server, p1, amount);

        server->player->carried = 0;
        server->player->pos = find_avb_pos(map, server->player->pos);
    }

    if(p2->y == b->y && p2->x == b->x && server->player[1].is_there == 1) {
        int amount = 0;
        amount += server->player[1].carried;
        if(amount != 0) add_grave(server, p2, amount);

        server->player[1].carried = 0;
        server->player[1].pos = find_avb_pos(map, server->player[1].pos);
    }

    for(int i = 0; i < 10; i++) {
        if(server->graves[i].pos.y != -1) {
            if(p1->y == server->graves[i].pos.y && p1->x == server->graves[i].pos.x) {
                server->player->carried += server->graves[i].amount;
                server->graves[i].amount = 0;
                server->graves[i].pos.y = -1;
                server->graves[i].pos.x = -1;
            }

            if(p2->y == server->graves[i].pos.y && p2->x == server->graves[i].pos.x) {
                server->player[1].carried += server->graves[i].amount;
                server->graves[i].amount = 0;
                server->graves[i].pos.y = -1;
                server->graves[i].pos.x = -1;
            }
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

        case 'D':
            break;

        case '#':
            break;

        default:
            *(*(map+player->pos->y)+player->pos->x) = ' ';
            break;
    }
}

char** player_vision(struct server *server, char **map, int pId) {
    char **vision = server->player[pId].view;

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            *(*(vision+i)+j) = 'B';

            if(server->player[pId].pos->y-2+i >= 0 && server->player[pId].pos->y-1+i <= MAP_HEIGHT &&
                    server->player[pId].pos->x-2+j >= 0 && server->player[pId].pos->x-1+j <= MAP_WIDTH) {
                *(*(vision+i)+j) = *(*(map+server->player[pId].pos->y-2+i)+server->player[pId].pos->x-2+j);
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

void *player2(void *arg)
{
    while(1) {
        struct server *server = (struct server*)arg;

        mkfifo("r_player", 0777);
        mkfifo("w_player", 0777);

        int r_fifo = open("r_player", O_RDONLY);
        int w_fifo = open("w_player", O_WRONLY);

        (server->player+1)->is_there = 1;

        for(;;testsik++) {
            pthread_mutex_lock(&(server->player+1)->player_m);
            send_struct(server, w_fifo);

            read(r_fifo, &p2In, sizeof(int));

            if(p2In == 'q' || p2In == 'Q') {
                (server->player+1)->is_there = 0;
                break;
            }

            server->player[1].pos = move_check(p2In, server->player+1, server->player[1].view);
        }

        close(r_fifo);
        close(w_fifo);

        if(quitFlag == 'q' || quitFlag == 'Q') {
            break;
        }
    }
}

void send_struct(struct server *server, int w_fifo)
{
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            char tmp = *(*((server->player+1)->view+i)+j);
            write(w_fifo, &tmp, sizeof(char));
        }
    }

    for(int i = 0; i < 10; i++) {
        int x, y, a;
        x = server->graves[i].pos.x;
        y = server->graves[i].pos.y;

        a = server->graves[i].amount;

        write(w_fifo, &x, sizeof(int));
        write(w_fifo, &y, sizeof(int));
        write(w_fifo, &a, sizeof(int));
    }

    int tmp = server->player->pos->x;
    write(w_fifo, &tmp, sizeof(int));
    tmp = server->player->pos->y;
    write(w_fifo, &tmp, sizeof(int));

    tmp = server->player[1].pos->x;
    write(w_fifo, &tmp, sizeof(int));
    tmp = server->player[1].pos->y;
    write(w_fifo, &tmp, sizeof(int));

    tmp = server->player[1].bank;
    write(w_fifo, &tmp, sizeof(int));
    tmp = server->player[1].carried;
    write(w_fifo, &tmp, sizeof(int));

    tmp = server->pId;
    write(w_fifo, &tmp, sizeof(int));

    tmp = server->beast->pos->x;
    write(w_fifo, &tmp, sizeof(int));
    tmp = server->beast->pos->y;
    write(w_fifo, &tmp, sizeof(int));

    tmp = 0;

    if(quitFlag == 'q' || quitFlag == 'Q') {
        tmp = 1;
    }

    write(w_fifo, &tmp, sizeof(int));
}
