#include "../headers/main.h"

int main(int argc, char const *argv[]) {
    gameStruct *game = gameInit();
    int continueMenu = 1;
    int continueGame = 1;
    int correct_input = 1;
    int lengthInfos = 0;
    int continueServer;
    Uint32 secondsMenu = SDL_GetTicks();
    while (continueMenu) {
        menuDraw(game, &secondsMenu);
        continueMenu = menuEvent(game);
        correct_input = 1;

        // reset port an ip if the loop comes back here
        memset(game->infos->port, '\0', 8);
        memset(game->infos->ipAdresse, '\0', 24);
        game->infos->port[0] = ' ';
        game->infos->ipAdresse[0] = ' ';

        // 4 = server
        if (continueMenu == 4) {
            SDL_StartTextInput();
            game->menu->inputTexture = changeText_(game, game->infos->port, 24);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do{
                menuDraw(game, &secondsMenu);
                game->menu->inputSelected = menuTextInputEvent(game->infos->port, game, 7);
                game->menu->inputTexture = changeText_fixedSize(game, game->infos->port);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            }while(game->menu->inputSelected);
            game->menu->menuSelected = 4;
            game->menu->inputTexture = changeText_(game, game->infos->ipAdresse, 20);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do {
                menuDraw(game, &secondsMenu);
                game->menu->inputSelected = menuTextInputEvent(game->infos->ipAdresse, game, 17);
                game->menu->inputTexture = changeText_fixedSize(game, game->infos->ipAdresse);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            } while(game->menu->inputSelected);
            if (game->infos->port[1] == '\0') { // gestion du default
                memset(game->infos->port, '\0', 8);
                strcpy(game->infos->port, ":5000");
            } else {
                for(int i = 1; i < strlen(game->infos->port); i++)
                {
                    if (game->infos->port[i] == 46) {
                        correct_input = 0;
                    }
                }
            }
            if (game->infos->ipAdresse[1] == '\0') { // gestion du default
                memset(game->infos->ipAdresse, '\0', 24);
                strcpy(game->infos->ipAdresse, ":127.0.0.1");
            } else {
                for(int i = 1; i < strlen(game->infos->ipAdresse); i++)
                {
                    if (game->infos->ipAdresse[i] == 46) {
                        if (game->infos->ipAdresse[i+1] == 46) {
                            correct_input = 0;
                        }
                        lengthInfos++;
                    }
                }
                if (lengthInfos != 3) {
                    correct_input = 0;
                    puts("Wrong ip input");
                    printf("ip : %s\n", game->infos->ipAdresse);
                }
            }
            if (!correct_input) {
                puts("Wrong port input or ip input");
                game->menu->inputSelected = -1;
            }
            else {
                SDL_StopTextInput();
                game->mapCaracteristics->mapNumber = 0;
                printf("--map number : %d\n\n", game->mapCaracteristics->mapNumber);
                game->infos->portInt = atoi(game->infos->port+1);

                // creating server thred and connect to it with a client
                pthread_t sniffer_thread;
                if( pthread_create( &sniffer_thread, NULL, theServer, (void*)game) < 0) {
                    perror("Server thread creation problem");
                    return 1;
                }
                puts("Server thread created");
                game = mapInitServer(game->numberOfBlocks_X, game->numberOfBlocks_Y, game);
                puts("New map initialised");
                // int switchEnvoyerPlayersPositions = 0;
                // SDL_Delay(500);
                // theClient("127.0.0.1", game->infos->portInt, game);

                // int currentMaxConnexions = game->sockets->maxConnexions;
                do // general loop after server start
                {
                    continueServer = eventInsideServer();
                    menuDraw_serverRunning(game);
                    // currentMaxConnexions = playerNumberChangeMonitor(game, currentMaxConnexions);
                } while (continueServer);

                // terminating server thread
                if(pthread_detach(sniffer_thread) != 0){
                    puts("problem terminating server thread!");
                } else {
                    puts("terminated server thread succesfully!");
                }
            }
            game->menu->menuSelected = 1;
            game->menu->inputSelected = 0;
        // 3 = client
        } else if (continueMenu == 3) {
            SDL_StartTextInput();
            game->menu->inputTexture = changeText_(game, game->infos->port, 20);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do{
                menuDraw(game, &secondsMenu);
                game->menu->inputSelected = menuTextInputEvent(game->infos->port, game, 7);
                game->menu->inputTexture = changeText_fixedSize(game, game->infos->port);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            } while(game->menu->inputSelected);
            game->menu->menuSelected = 4;
            game->menu->inputTexture = changeText_(game, game->infos->ipAdresse, 20);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do {
                menuDraw(game, &secondsMenu);
                game->menu->inputSelected = menuTextInputEvent(game->infos->ipAdresse, game, 17);
                game->menu->inputTexture = changeText_fixedSize(game, game->infos->ipAdresse);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            } while(game->menu->inputSelected);
            if (game->infos->port[1] == '\0') { // gestion du default
                memset(game->infos->port, '\0', 8);
                strcpy(game->infos->port, ":5000");
            } else {
                for(int i = 1; i < strlen(game->infos->port); i++)
                {
                    if (game->infos->port[i] == 46) {
                        correct_input = 0;
                    }
                }
            }
            if (game->infos->ipAdresse[1] == '\0') { // gestion du default
                memset(game->infos->ipAdresse, '\0', 24);
                strcpy(game->infos->ipAdresse, ":127.0.0.1");
            } else {
                for(int i = 1; i < strlen(game->infos->ipAdresse); i++)
                {
                    if (game->infos->ipAdresse[i] == 46) {
                        if (game->infos->ipAdresse[i+1] == 46) {
                            correct_input = 0;
                        }
                        lengthInfos++;
                    }
                }
                if (lengthInfos != 3) {
                    correct_input = 0;
                    puts("Wrong ip input");
                    printf("ip : %s\n", game->infos->ipAdresse);
                }
            }
            if (correct_input) {
                SDL_StopTextInput();
                game->infos->portInt = atoi(game->infos->port+1);
                game->infos->ipAdresse;
                theClient(game->infos->ipAdresse+1, game->infos->portInt, game);
            } else {
                puts("Wrong port input or ip input");
                game->menu->inputSelected = -1;
            }
            game->menu->menuSelected = 1;
            game->menu->inputSelected = 0;
            lengthInfos = 0;
        }
        //SDL_Delay(80);
    }
    gameDestroy(game);
    return 0;
}
