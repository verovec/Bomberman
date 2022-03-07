#include "../headers/main.h"

int theClient(char *ip, int port, gameStruct *game){
    char *message = malloc(sizeof(char) * 512);
    struct sockaddr_in addr_server;
    int playerNumber; int playersThatAreHere[4] = {0,0,0,0};
    int playerToMoveNumber; int direction; int speacialCommand; int movementNumber = 0; int movementNumber_backup = 0;
    int animationsArray[16][4];
    Uint32 secondsAnimations[16];
    Uint32 secondsAnimationsBonuses = SDL_GetTicks();

    addr_server.sin_addr.s_addr = inet_addr(ip);
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port);

    int the_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (the_socket < 0) {
        perror ("socket()");
        return -1;
    }
    if (connect(the_socket, (struct sockaddr*)&addr_server, sizeof(addr_server)) < 0)  {
        perror ("connect()");
        game->menu->inputSelected = -2;
        return 1;
    }

    // reception du player number et des players presents
    memset(message, '\0', 512);
    read(the_socket, message, 16);
    printf("--player number : %s\n\n", message);
    playerNumber = message[0] - 48;
    for(int i = 0; i < 4; i++)
    {
        playersThatAreHere[i] = message[i + 1] - 48;
    }
    printf("--map number : %c\n\n", message[5]);
    game->mapCaracteristics->mapNumber = message[5] - 48;
    for(int j = 0; j < 16; j++)
    {
        animationsArray[j][0] = -1;
    }
    memset(message, '\0', 32);
    SDL_Event e;
    int continueGame;
    Uint32 seconds = SDL_GetTicks();
    do {
        int playersonline = 0;
        for(int i = 0; i < 4; i++) {
            if(playersThatAreHere[i] == 1) {
                playersonline++;
            }
        }
        gameDraw(game, game->numberOfBlocks_X, game->numberOfBlocks_Y, playersThatAreHere);
        continueGame = gameEventClient_sendMessage(game, &seconds, playerNumber, message, playersonline);

        if (message[0] != '\0') {
            send(the_socket, message, strlen(message), MSG_NOSIGNAL);
            // printf("--sended : %s\n", message);
            memset(message, '\0', 32);
        }
        recv(the_socket, message, 512, MSG_DONTWAIT);
        if (message[0] != '\0') {
            printf("---- recieved : %s\n", message);
            movementNumber_backup = movementNumber;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, &seconds, movementNumber, &playerToMoveNumber, &direction, &speacialCommand, the_socket);
            if (movementNumber_backup < movementNumber) {
                for(int i = 0; i < 16; i++)
                {
                    if (animationsArray[i][0] == -1) {
                        animationsArray[i][0] = playerToMoveNumber;
                        animationsArray[i][1] = direction;
                        animationsArray[i][2] = 0;
                        animationsArray[i][3] = speacialCommand;
                        secondsAnimations[i] = SDL_GetTicks();
                        i = 16;
                    }
                }
            } else {
                movementNumber = movementNumber_backup;
            }
        }
        animations(game, animationsArray, secondsAnimations);
        bonusAnimations(game, &secondsAnimationsBonuses);
        memset(message, '\0', 512);
    } while(continueGame);

    // terminating client thread and freeing things
    close(the_socket);
    free(message);
    puts("will try to detach");
    if(pthread_detach(pthread_self()) != 0){
        puts("problem terminating thread! (Client)");
    } else {
        puts("terminated thread (Client) succesfully!");
    }
    return 0;
}
