#include "../headers/main.h"

#define playerX game->players[num]->playerSquarePosition.x
#define playerY game->players[num]->playerSquarePosition.y

// return 5 for the bombe and 1,2,3,4 for the directions in the fonction gameMovePlayer
int gameEventClient_sendMessage(gameStruct *game, Uint32 *seconds, int playerNumber, char *message, int playersonline) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 0;
        } else {
            if (playersonline < 2) {
                printf("%s", "en attente");

                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            strcpy(message, "0\n");
                            puts("quit");
                            return 0;
                            break;

                        default:
                            //strcpy(message, "9");
                            printf("Key pressed not recognised : %d\n", e.key.keysym.sym);
                            break;
                    }
                }
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        strcpy(message, "0\n");
                        puts("quit");
                        return 0;
                        break;

                    case SDLK_SPACE:
                        //generateBombeClient(game, playerNumber);
                        strcpy(message, "5");
                        puts("bombe");
                        break;

                    case SDLK_UP:
                        strcpy(message, "1");
                        game->players[playerNumber]->direction = 0;
                        puts("up");
                        break;

                    case SDLK_DOWN:
                        strcpy(message, "2");
                        game->players[playerNumber]->direction = 1;
                        puts("down");
                        break;

                    case SDLK_RIGHT:
                        strcpy(message, "3");
                        game->players[playerNumber]->direction = 2;
                        puts("right");
                        break;

                    case SDLK_LEFT:
                        strcpy(message, "4");
                        game->players[playerNumber]->direction = 3;
                        puts("left");
                        break;

                    default:
                        //strcpy(message, "9");
                        printf("Key pressed not recognised : %d\n", e.key.keysym.sym);
                        break;
                }
            }
        }
    }
    //game->player->playerSpritePosition.x = 0;
    return 6;
}

int gameMovePlayer_client(gameStruct *game, int direction, Uint32 *seconds, int num, int speacialCommand) {
        if (direction == 1) {
            game->players[num]->playerSquarePosition.y--;
            game->players[num]->direction = 0;
            return 1;
        } else if (direction == 2) {
            game->players[num]->playerSquarePosition.y++;
            game->players[num]->direction = 1;
            return 2;
        } else if (direction == 3) {
            game->players[num]->playerSquarePosition.x++;
            game->players[num]->direction = 2;
            return 3;
        } else if (direction == 4) {
            game->players[num]->playerSquarePosition.x--;
            game->players[num]->direction = 3;
            return 4;
        } else if (direction == 5) {
            game->players[num]->playerBombes->activated[speacialCommand] = 1;
            game->players[num]->playerStats->numberOfBombes--;
            game->players[num]->playerBombes->bombeSquarePosition[speacialCommand].x = game->players[num]->playerSquarePosition.x;
            game->players[num]->playerBombes->bombeSquarePosition[speacialCommand].y = game->players[num]->playerSquarePosition.y;
            game->players[num]->playerBombes->bombePosition[speacialCommand].x = 48 + ((game->players[num]->playerSquarePosition.x-1) * 48);
            game->players[num]->playerBombes->bombePosition[speacialCommand].y = 48 + ((game->players[num]->playerSquarePosition.y-1) * 48);

            game->players[num]->playerBombes->bombesOnWhichSquare[speacialCommand][0] = game->players[num]->playerSquarePosition.y;
            game->players[num]->playerBombes->bombesOnWhichSquare[speacialCommand][1] = game->players[num]->playerSquarePosition.x;
            return 5;
        }
    return 6;
}


int gameEventClient_recieveMessage(gameStruct *game, char *message, int playersThatAreHere[4], Uint32 *seconds, int movementNumber, int *playerToMoveNumber, int *direction, int *speacialCommand, int the_socket){
    if (message[0] == '!') {  // for commands
        *direction = message[1] - 48;
        *playerToMoveNumber = message[2] - 48;
        gameMovePlayer_client(game, *direction, seconds, *playerToMoveNumber, 0);
        if (message[3] != '\0' && message[3] != '(') {
            bonusTaking_Client(game, *playerToMoveNumber, message);
        } else if (message[3] != '\0'){
            message+=3;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        movementNumber++;
        return movementNumber;
    } else if (message[0] == '$') {  // for bombe
        *direction = 5;
        *playerToMoveNumber = message[2] - 48;
        *speacialCommand = message[3] - 48;
        gameMovePlayer_client(game, *direction, seconds, *playerToMoveNumber, *speacialCommand);
        if (message[4] != '\0') {
            message+=4;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        movementNumber++;
        return movementNumber;
    } else if (message[0] == '%') {  // for number of players
        printf("players number message : %s\n", message);
        for(int j = 0; j < 4; j++)
        {
            playersThatAreHere[j] = message[j + 1] - 48;
        }
        if (message[5] != '\0') {
            message+=5;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        puts("players number updated");
        return movementNumber;
    } else if (message[0] == '\'') {  // for players positions (sents when someone new connects so he knows about the position of other players)
        printf("players position message : %s\n", message);
        int j = 1;
        for(int k = 0; k < 4; k++)
        {
            if (playersThatAreHere[k] > 0) {
                if (game->players[k]->playerSquarePosition.x != message[j] - 48) {
                    game->players[k]->playerSquarePosition.x = message[j] - 48;
                    game->players[k]->playerPosition.x = 48 + ((game->players[k]->playerSquarePosition.x-1) * 48);
                    movementNumber--;
                }
                if (game->players[k]->playerSquarePosition.y != message[j+1] - 48) {
                    game->players[k]->playerSquarePosition.y = message[j+1] - 48;
                    game->players[k]->playerPosition.y = 30 + ((game->players[k]->playerSquarePosition.y-1) * 48);
                    movementNumber--;
                }
                if (game->players[k]->playerStats->lives != message[j+2] - 48) {
                    game->players[k]->playerStats->lives = message[j+2] - 48;
                }
                if (game->players[k]->playerStats->speed != (message[j+3] - 48) * 10) {
                    game->players[k]->playerStats->speed = (message[j+3] - 48) * 10;
                }
                if (game->players[k]->playerStats->numberOfBombes != message[j+4] - 48) {
                    game->players[k]->playerStats->numberOfBombes = message[j+4] - 48;
                }
                if (game->players[k]->playerStats->numberOfBombesPlanted != message[j+5] - 48) {
                    game->players[k]->playerStats->numberOfBombesPlanted = message[j+5] - 48;
                }
            }
            j+=6;
        }
        if (message[25] != '\0') {
            message+=25;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        puts("players positions updated");
        return movementNumber;
    } else if (message[0] == '\"') { // for initial explosion
        for(int k = 0; k < 4; k++){
            game->players[k]->playerStats->lives = message[k+1] - 48;
            if (game->players[k]->playerStats->lives == 0) {
                printf("player %d is dead\n", k);
            }
        }
        *direction = 6;
        *playerToMoveNumber = message[5] - 48;
        *speacialCommand = message[6] - 48;
        game->players[*playerToMoveNumber]->playerBombes->activated[*speacialCommand] = 0;
        game->players[*playerToMoveNumber]->playerBombes->exploded[*speacialCommand] = 1;
        movementNumber++;
        int i = 7;
        while(message[i] != '$' && message[i] != '\0'){
            if ((message[i]-48)%2 == 1 && (message[i+1]-48)%2 == 1) {
                game->map[message[i]-48][message[i+1]-48]->squareType = 1;
            } else {
                game->map[message[i]-48][message[i+1]-48]->squareType = 0;
            }
            i+=2;
        }
        if (message[i] == '$') {
            i++;
        }
        int k;
        while(message[i] != '\0' && message[i] != '%' && message[i] != '\'' && message[i] != '(' && message[i] != '!' && message[i] != '$' && message[i] != '\"' && message[i] != '#' && message[i] != ')' && message[i] != '*'){
            k = message[i] - 48;
            game->bonuses->bonusesArray[k]->bonusType = message[i+1] - 48;
            printf("bonus type : %d\n", game->bonuses->bonusesArray[k]->bonusType);
            game->bonuses->bonusesArray[k]->bonusSquarePosition.x = message[i+2] - 48;
            game->bonuses->bonusesArray[k]->bonusSquarePosition.y = message[i+3] - 48;
            printf("bonusX : %d | bonusY : %d\n", game->bonuses->bonusesArray[k]->bonusSquarePosition.x, game->bonuses->bonusesArray[k]->bonusSquarePosition.y);
            game->bonuses->bonusesArray[k]->bonusPosition.x = game->map[game->bonuses->bonusesArray[k]->bonusSquarePosition.y][game->bonuses->bonusesArray[k]->bonusSquarePosition.x]->squarePosition.x;
            game->bonuses->bonusesArray[k]->bonusPosition.y = game->map[game->bonuses->bonusesArray[k]->bonusSquarePosition.y][game->bonuses->bonusesArray[k]->bonusSquarePosition.x]->squarePosition.y;
            game->bonuses->bonusesArray[k]->bonusSpritePosition.x = 0; game->bonuses->bonusesArray[k]->bonusSpritePosition.y = 0;
            game->bonuses->bonusesArray[k]->bonusSpritePosition.h = 64; game->bonuses->bonusesArray[k]->bonusSpritePosition.w = 64;
            i+=4;
        }
        if (message[i] != '\0') {
            message+=i;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        return movementNumber;
    } else if (message[0] == '#') { // for explosion
        for(int k = 0; k < 4; k++){
            game->players[k]->playerStats->lives = message[k+1] - 48;
            if (game->players[k]->playerStats->lives == 0) {
                printf("player %d is dead\n", k);
            }
        }
        if (message[5] != '\0') {
            message+=5;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        return movementNumber;
    } else if (message[0] == '(') { // for map
        printf("message map : %s\n", message);
        int l = 1;
        while(message[l] != '\0' && message[l] != '%' && message[l] != '\'' && message[l] != '(' && message[l] != '!' && message[l] != '$' && message[l] != '\"' && message[l] != '#' && message[l] != ')' && message[l] != '*'){
            if (message[l] - 48 == 3) {
                game->map[message[l+1] - 48][message[l+2] - 48]->squareType = 2;
            }
            l+=3;
        }
        if (message[l] != '\0') {
            message+=l;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        // memset(message, '\0', 255);
        // write(the_socket , "(\0", 16);
    } else if (message[0] == ')') {
        printf("message bonuses array : %s\n", message);
        int i = 1;
        while(message[i] != '\0' && message[i] != '%' && message[i] != '\'' && message[i] != '(' && message[i] != '!' && message[i] != '$' && message[i] != '\"' && message[i] != '#' && message[i] != ')' && message[i] != '*'){
            game->bonuses->bonusesArray[message[i] - 48]->bonusType = message[i+1] - 48;
            game->bonuses->bonusesArray[message[i] - 48]->bonusSquarePosition.x = message[i+2] - 48;
            game->bonuses->bonusesArray[message[i] - 48]->bonusSquarePosition.y = message[i+3] - 48;
            game->bonuses->bonusesArray[message[i] - 48]->bonusSpritePosition.x = 0; game->bonuses->bonusesArray[message[i] - 48]->bonusSpritePosition.y = 0;
            game->bonuses->bonusesArray[message[i] - 48]->bonusSpritePosition.h = 64; game->bonuses->bonusesArray[message[i] - 48]->bonusSpritePosition.w = 64;
            game->bonuses->bonusesArray[message[i] - 48]->bonusPosition.x = game->map[game->bonuses->bonusesArray[message[i] - 48]->bonusSquarePosition.y][game->bonuses->bonusesArray[message[i] - 48]->bonusSquarePosition.x]->squarePosition.x;
            game->bonuses->bonusesArray[message[i] - 48]->bonusPosition.y = game->map[game->bonuses->bonusesArray[message[i] - 48]->bonusSquarePosition.y][game->bonuses->bonusesArray[message[i] - 48]->bonusSquarePosition.x]->squarePosition.y;
            i += 4;
        }
        if (message[i] != '\0') {
            message+=i;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
        // memset(message, '\0', 255);
        // write(the_socket , ")\0", 16);
    } else if (message[0] == '*') {
        puts("updating the player stats");
        int j = 1;
        char *stat = malloc(sizeof(char)*4);
        memset(stat, '\0', 4);
        for(size_t i = 0; i < 4; i++)
        {
            game->players[i]->playerStats->lives = message[j+1] - 48;
            game->players[i]->playerStats->speed = (message[j+2] - 48) * 50;
            game->players[i]->playerStats->numberOfBombes = message[j+3] - 48;
            game->players[i]->playerStats->numberOfBombesPlanted = message[j+4] - 48;
            game->players[i]->playerStats->radiusOfBombes = message[j+5] - 48;

            memset(stat, '\0', 4);
            sprintf(stat, "%d", game->players[i]->playerStats->numberOfBombes);
            game->hud[i]->numberOfBombesTexture = changeText_fixedSize_12(game, stat);
            SDL_QueryTexture(game->hud[i]->numberOfBombesTexture, NULL, NULL, &game->hud[i]->numberOfBombesPosition.w, &game->hud[i]->numberOfBombesPosition.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->playerStats->speed);
            game->hud[i]->speedTexture = changeText_fixedSize_12(game, stat);
            SDL_QueryTexture(game->hud[i]->speedTexture, NULL, NULL, &game->hud[i]->speedPosition.w, &game->hud[i]->speedPosition.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->playerStats->radiusOfBombes);
            game->hud[i]->radiusOfBombesTexture = changeText_fixedSize_12(game, stat);
            SDL_QueryTexture(game->hud[i]->radiusOfBombesTexture, NULL, NULL, &game->hud[i]->radiusOfBombesPosition.w, &game->hud[i]->radiusOfBombesPosition.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->playerStats->lives);
            game->hud[i]->aliveTexture = changeText_fixedSize_12(game, stat);
            SDL_QueryTexture(game->hud[i]->aliveTexture, NULL, NULL, &game->hud[i]->alivePosition.w, &game->hud[i]->alivePosition.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->playerStats->numberOfBombesPlanted);
            game->hud[i]->numberOfBombesPlantedTexture = changeText_fixedSize_12(game, stat);
            SDL_QueryTexture(game->hud[i]->numberOfBombesPlantedTexture, NULL, NULL, &game->hud[i]->numberOfBombesPlantedPosition.w, &game->hud[i]->numberOfBombesPlantedPosition.h);

            j+=6;
        }
        if (message[j] != '\0') {
            message+=j;
            movementNumber = gameEventClient_recieveMessage(game, message, playersThatAreHere, seconds, movementNumber, playerToMoveNumber, direction, speacialCommand, the_socket);
        }
    }
    return movementNumber;
}

int bonusTaking_Client(gameStruct *game, int num, char *message){
    char *stat = malloc(sizeof(char)*4);
    memset(stat, '\0', 4);
    // char *backSlachZero = "\0";
    // strcat(stat, backSlachZero);
    game->bonuses->bonusesArray[message[3] - 48]->bonusType = -1;
    if (message[4] - 48 == 0) {
        game->players[num]->playerStats->numberOfBombes = message[5] - 48;
        sprintf(stat, "%d", game->players[num]->playerStats->numberOfBombes);
        game->hud[num]->numberOfBombesTexture = changeText_fixedSize_12(game, stat);
        SDL_QueryTexture(game->hud[num]->numberOfBombesTexture, NULL, NULL, &game->hud[num]->numberOfBombesPosition.w, &game->hud[num]->numberOfBombesPosition.h);
    } else if (message[4] - 48 == 1) {
        game->players[num]->playerStats->speed = (message[5] - 48) * 50;
        sprintf(stat, "%d", game->players[num]->playerStats->speed);
        game->hud[num]->speedTexture = changeText_fixedSize_12(game, stat);
        SDL_QueryTexture(game->hud[num]->speedTexture, NULL, NULL, &game->hud[num]->speedPosition.w, &game->hud[num]->speedPosition.h);
    } else if (message[4] - 48 == 2) {
        game->players[num]->playerStats->radiusOfBombes = message[5] - 48;;
        sprintf(stat, "%d", game->players[num]->playerStats->radiusOfBombes);
        game->hud[num]->radiusOfBombesTexture = changeText_fixedSize_12(game, stat);
        SDL_QueryTexture(game->hud[num]->radiusOfBombesTexture, NULL, NULL, &game->hud[num]->radiusOfBombesPosition.w, &game->hud[num]->radiusOfBombesPosition.h);
    } else if (message[4] - 48 == 3) {
        game->players[num]->playerStats->lives = message[5] - 48;
        sprintf(stat, "%d", game->players[num]->playerStats->lives);
        game->hud[num]->aliveTexture = changeText_fixedSize_12(game, stat);
        SDL_QueryTexture(game->hud[num]->aliveTexture, NULL, NULL, &game->hud[num]->alivePosition.w, &game->hud[num]->alivePosition.h);
    } else if (message[4] - 48 == 4) {
        game->players[num]->playerStats->numberOfBombesPlanted = message[5] - 48;
        sprintf(stat, "%d", game->players[num]->playerStats->numberOfBombesPlanted);
        game->hud[num]->numberOfBombesPlantedTexture = changeText_fixedSize_12(game, stat);
        SDL_QueryTexture(game->hud[num]->numberOfBombesPlantedTexture, NULL, NULL, &game->hud[num]->numberOfBombesPlantedPosition.w, &game->hud[num]->numberOfBombesPlantedPosition.h);
    }
}
