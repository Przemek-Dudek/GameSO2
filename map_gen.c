#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "functions.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

struct pos *find_avb_pos(char **map, struct pos *position)
{
    static int check = 0;

    position->x = rand()%MAP_WIDTH;
    position->y = rand()%MAP_HEIGHT;

    check++;
    if(check >= 10000) {
        free(position);
        return NULL;
    }
    if(*(*(map+position->y)+position->x) != ' ') return find_avb_pos(map, position);

    return position;
}

int move_pos(struct pos *cur, char **map, int step)
{
    //calculate weights
    int weight_xR = MAP_WIDTH/2;
    int weight_xL = MAP_WIDTH/2;
    int weight_yD = MAP_HEIGHT/3;
    int weight_yU = MAP_HEIGHT/3;

    for(int i = 1; i < MIN(MAP_WIDTH, MAP_HEIGHT); i++) {
        if(cur->x+i < MAP_WIDTH-1){
            if(*(*(map+cur->y)+cur->x+i) == ' ') weight_xR--;
        }

        if(cur->x-i >= 1) {
            if(*(*(map+cur->y)+cur->x-i) == ' ') weight_xL--;
        }

        if(cur->y+i < MAP_HEIGHT-1) {
            if(*(*(map+cur->y+i)+cur->x) == ' ') weight_yD--;
        }

        if(cur->y-i >= 1) {
            if(*(*(map+cur->y-i)+cur->x) == ' ') weight_yU--;
        }
    }

    if(weight_xR <= 0) {
        weight_xL -= weight_xR;
        weight_xR = 1;
    }

    if(weight_xL <= 0) {
        weight_xR -= weight_xL;
        weight_xL = 1;
    }

    if(weight_yU <= 0) {
        weight_yD -= weight_yU;
        weight_yU = 1;
    }

    if(weight_yD <= 0) {
        weight_yU -= weight_yD;
        weight_yD = 1;
    }

    //Random number generation
    int num = rand()%(weight_xR+weight_xL+weight_yD+weight_yU);

    //Opcja kejwowa
    if(step == 1) {
        if(num < weight_xR) {
            //move Right
            if(cur->x+step >= MAP_WIDTH-1) return move_pos(cur, map, step);

            cur->x += 1;

            return 0;

        } else if(num < weight_xR+weight_xL) {
            //move Left

            if(cur->x <= step) return move_pos(cur, map, step);

            cur->x -= 1;

            return 1;

        } else if(num < weight_xR+weight_xL+weight_yU) {
            //move Up

            if(cur->y <= step) return move_pos(cur, map, step);

            cur->y -= 1;

            return 2;

        } else if(num < weight_xR+weight_xL+weight_yU+weight_yD) {
            //move Down

            if(cur->y+step >= MAP_HEIGHT-1) return move_pos(cur, map, step);

            cur->y += 1;

            return 3;
        }
    }

    if(num < weight_xR) {
        //move Right
        if(cur->x+step >= MAP_WIDTH) return move_pos(cur, map, step);

        cur->x += 1;

        return 0;

    } else if(num < weight_xR+weight_xL) {
        //move Left

        if(cur->x <= step-1) return move_pos(cur, map, step);

        cur->x -= 1;

        return 1;

    } else if(num < weight_xR+weight_xL+weight_yU) {
        //move Up

        if(cur->y <= step-1) return move_pos(cur, map, step);

        cur->y -= 1;

        return 2;

    } else if(num < weight_xR+weight_xL+weight_yU+weight_yD) {
        //move Down

        if(cur->y+step >= MAP_HEIGHT) return move_pos(cur, map, step);

        cur->y += 1;

        return 3;
    }
}

void krzak_populate(char **map, int step)
{
    for(int i = 0; i < MAP_HEIGHT; i++) {
        for(int j = 0; j < MAP_WIDTH; j++) {
            if(*(*(map+i)+j) == ' ') {
                if(rand()%(2*(6-step)) == 0) *(*(map+i)+j) = '#';
            }
        }
    }
}

void display(char **map) {
    for(int i = 0; i < MAP_HEIGHT; i++) {
        for(int j = 0; j < MAP_WIDTH; j++) {
            printf("%c", *(*(map+i)+j));
        }
        printf("\n");
    }
    printf("\n");
}

int map_gen()
{
    FILE *f = fopen(filename, "w");
    if(f == NULL)
        return -1;

    srand(time(NULL));

    int step;

    if(rand()%16 == 0) {
        step = 1;
    } else step = 2;

    for(int i = 0; i < NUM_OF_MAPS; i++) {

        //Map generation
        char **map;

        map = calloc(MAP_HEIGHT, sizeof(char*));

        for(int j = 0; j < MAP_HEIGHT; j++) {
            *(map+j) = calloc(MAP_WIDTH, sizeof(char));
        }

        for(int j = 0; j < MAP_HEIGHT; j++) {
            for(int k = 0; k < MAP_WIDTH; k++) {
                *(*(map+j)+k) = 'W';
            }
        }

        //pos placement
        struct pos *cur;

        cur = calloc(1, sizeof(struct pos));

        cur->x = MAP_WIDTH/2;
        cur->y = MAP_HEIGHT/2;

        //Number of steps
        int num_of_steps = (MAP_HEIGHT + MAP_WIDTH)*MAX(MAP_HEIGHT, MAP_WIDTH)*6/step;

        //path mapping
        *(*(map+cur->y)+cur->x) = ' ';

        for(int j = 0; j < num_of_steps; j++) {

            int tmp = move_pos(cur, map, step);

            *(*(map+cur->y)+cur->x) = ' ';

            for(int k = 0; k < step-1; k++) {
                switch (tmp) {
                    case 0:
                        if(cur->x < MAP_WIDTH-2) cur->x += 1;
                        break;

                    case 1:
                        if(cur->x > 2) cur->x -= 1;
                        break;

                    case 2:
                        if(cur->y > 2) cur->y -= 1;
                        break;

                    case 3:
                        if(cur->y < MAP_HEIGHT-2) cur->y += 1;
                        break;
                }

                *(*(map+cur->y)+cur->x) = ' ';
            }
        }

        //Krzak_populate

        krzak_populate(map, step);

        //Camp setup

        struct pos *avb_pos = malloc(sizeof(struct pos));
        avb_pos = find_avb_pos(map, avb_pos);

        *(*(map+avb_pos->y)+avb_pos->x) = 'A';

        //Coin&Treasure tossing

        for(int j = 0; j < NUM_OF_TRESURE; j++) {
            avb_pos = find_avb_pos(map, avb_pos);

            *(*(map+avb_pos->y)+avb_pos->x) = 'T';
        }

        for(int j = 0; j < NUM_OF_LARGE_TRESURE; j++) {
            avb_pos = find_avb_pos(map, avb_pos);

            *(*(map+avb_pos->y)+avb_pos->x) = 't';
        }

        for(int j = 0; j < NUM_OF_COINS; j++) {
            avb_pos = find_avb_pos(map, avb_pos);

            *(*(map+avb_pos->y)+avb_pos->x) = 'c';
        }

        //saving to file

        for(int j = 0; j < MAP_HEIGHT; j++) {
            for(int k = 0; k < MAP_WIDTH; k++) {
                fprintf(f, "%c", *(*(map+j)+k));
            }
            fputc('\n', f);
        }

        fputc('\n', f);

        if(DISPLAY_MAPS == 1) {
            display(map);
        }
        free(avb_pos);
        free(cur);
        free_map(map);
    }

    fclose(f);
    return 0;
}

void spawn_coin(char ***map)
{

    struct pos *avb_pos = malloc(sizeof(struct pos));
    avb_pos = find_avb_pos(*map, avb_pos);
    if(avb_pos == NULL) return;

    *(*(*map+avb_pos->y)+avb_pos->x) = 'c';

    free(avb_pos);
}

void spawn_treasure(char ***map)
{
    struct pos *avb_pos = malloc(sizeof(struct pos));
    avb_pos = find_avb_pos(*map, avb_pos);
    if(avb_pos == NULL) return;

    *(*(*map+avb_pos->y)+avb_pos->x) = 't';

    free(avb_pos);
}

void spawn_large_treasure(char ***map)
{
    struct pos *avb_pos = malloc(sizeof(struct pos));
    avb_pos = find_avb_pos(*map, avb_pos);
    if(avb_pos == NULL) return;

    *(*(*map+avb_pos->y)+avb_pos->x) = 'T';

    free(avb_pos);
}

void spawn_bestia(char ***map)
{
    struct pos *avb_pos = malloc(sizeof(struct pos));
    avb_pos = find_avb_pos(*map, avb_pos);
    if(avb_pos == NULL) return;

    *(*(*map+avb_pos->y)+avb_pos->x) = '*';

    //czary mary z wątkami
}