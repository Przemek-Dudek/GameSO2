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

    //mkfifo("player.c", 0777);

    char **map = NULL;
    struct server *server;

    if(prepServer(&map, &server) != 0) return 1;

    pthread_mutex_init(&server->player->player_m, NULL);
    pthread_mutex_init(&server->beast->beast_m, NULL);

    pthread_create(&server->player->player_t, NULL, player_handle, server->player);
    pthread_create(&server->beast->beast_t, NULL, beast_handle, server);

    for(int i = 0;; i++) {
        server->player->view = player_vision(server, map);
        server->beast->view = beast_vision(server, map);

        pthread_mutex_unlock(&server->player->player_m);
        pthread_mutex_unlock(&server->beast->beast_m);

        if(quitFlag == 'q' || quitFlag == 'Q') {
            break;
        }
        sleep(1);

        server->round = i;
        all_players_matter(server->player, map);
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