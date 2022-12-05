#include <ncurses.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "functions.h"
#include "functions.c"
#include "map_gen.c"

int main()
{
    initscr();
    start_color();
    keypad(stdscr, true);

    char **map = NULL;
    struct server *server;

    if(prepServer(&map, &server) != 0) return 1;

    pthread_t test;

    pthread_mutex_init(&server->player->player_m, NULL);
    pthread_mutex_init(&((server->player+1)->player_m), NULL);
    pthread_mutex_init(&server->beast->beast_m, NULL);


    pthread_create(&server->player->player_t, NULL, player_handle, server->player);
    pthread_create(&((server->player+1)->player_t), NULL, player2, server);
    pthread_create(&server->beast->beast_t, NULL, beast_handle, server);

    for(int i = 0; i != -1; i++) {
        if(quitFlag == 'q' || quitFlag == 'Q') {
            i = -1;
        }

        sleep(1);

        if(i == -1) break;

        player_vision(server, map, 0);
        player_vision(server, map, 1);
        server->beast->view = beast_vision(server, map);

        pthread_mutex_unlock(&server->player->player_m);
        pthread_mutex_unlock(&server->beast->beast_m);
        pthread_mutex_unlock(&((server->player+1)->player_m));

        server->round = i;
        all_players_matter(server->player, map);
        check_collision(server, map);
        all_players_matter(server->player+1, map);
        check_collision(server, map);
        display_map(map, server);
    }

    free(server->player->pos);
    for(int i = 0; i < 5; i++) {
        free(*(server->player->view+i));
    }
    free(server->player->view);
    free(server->player);

    free(server->beast->pos);
    for(int i = 0; i < 5; i++) {
        free(*(server->beast->view+i));
    }
    free(server->beast->view);
    free(server->beast);

    free(server);
    free_map(map);

    endwin();

    return 0;
}