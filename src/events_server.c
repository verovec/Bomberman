#include "../headers/main.h"

#define playerX game->players[num]->playerSquarePosition.x
#define playerY game->players[num]->playerSquarePosition.y
#define bombeX game->players[num]->playerBombes->bombeSquarePosition[bombeNum].x
#define bombeY game->players[num]->playerBombes->bombeSquarePosition[bombeNum].y

int eventServer(gameStruct *game, int direction, Uint32 *seconds, int num) {
    int commandInt = gameMovePlayer_server(game, direction, seconds, num);
    char *command = malloc(sizeof(char) * 512);
    memset(command, '\0', 512);
    if (commandInt != 99) {
        if (commandInt <= 4) {
            command[0] = '!'; // for command
            command[1] = commandInt + '0';
            command[2] = num + '0';
            bonusTaking(game, num, command);
            concatenateGameStat(game, command);
        } else if (commandInt <= 8) {
            command[0] = '$'; // for bombe
            command[1] = 5 + '0';
            command[2] = num + '0';
            int numOfTheBombe = commandInt - 5;
            command[3] = numOfTheBombe + '0'; // pour avoir sois 1 soit 0; le numero de la bombe; le retout se fait dans la fonction ci dessous avec 5 + i
        }
        for(int i = 0; i < 4; i++)
        {
            if (game->sockets->clientSockets[i] >= 0) {
                send(game->sockets->clientSockets[i], command, 512, MSG_NOSIGNAL);
            }
        }
         printf("------server sended : %s\n", command);
    }
    free(command);
}

int gameMovePlayer_server(gameStruct *game, int direction, Uint32 *seconds, int num) {
    Uint32 newSeconds = SDL_GetTicks();
    if (direction >= 0 && direction <= 4) {
        if (newSeconds >= *seconds+game->players[num]->playerStats->speed) {
            *seconds = newSeconds;
            int movingFactorX = game->screenSize.x / game->numberOfBlocks_X;
            int movingFactorY = game->screenSize.y / game->numberOfBlocks_Y;

            if (direction == 1) {
                if (game->players[num]->playerSquarePosition.y > 0 && game->map[playerY-1][playerX]->squareType < 2 && game->map[playerY-1][playerX]->playerOnSquare == 0 && game->map[playerY-1][playerX]->bombeOnSquare == 0) {
                    game->map[playerY][playerX]->playerOnSquare = 0;
                    game->map[playerY-1][playerX]->playerOnSquare = 1;
                    game->players[num]->playerSquarePosition.y--;
                    game->players[num]->playerPosition.y -= movingFactorY;
                    game->players[num]->direction = 0;
                    return 1;
                }
            } else if (direction == 2) {
                if (game->players[num]->playerSquarePosition.y < game->numberOfBlocks_Y-1 && game->map[playerY+1][playerX]->squareType < 2 && game->map[playerY+1][playerX]->playerOnSquare == 0 && game->map[playerY+1][playerX]->bombeOnSquare == 0) {
                    game->map[playerY][playerX]->playerOnSquare = 0;
                    game->map[playerY+1][playerX]->playerOnSquare = 1;
                    game->players[num]->playerSquarePosition.y++;
                    game->players[num]->playerPosition.y += movingFactorY;
                    game->players[num]->direction = 1;
                    return 2;
                }
            } else if (direction == 3) {
                if (game->players[num]->playerSquarePosition.x < game->numberOfBlocks_X-1 && game->map[playerY][playerX+1]->squareType < 2 && game->map[playerY][playerX+1]->playerOnSquare == 0 && game->map[playerY][playerX+1]->bombeOnSquare == 0) {
                    game->map[playerY][playerX]->playerOnSquare = 0;
                    game->map[playerY][playerX+1]->playerOnSquare = 1;
                    game->players[num]->playerSquarePosition.x++;
                    game->players[num]->playerPosition.x += movingFactorX;
                    game->players[num]->direction = 2;
                    return 3;
                }
            } else if (direction == 4) {
                if (game->players[num]->playerSquarePosition.x > 0 && game->map[playerY][playerX-1]->squareType < 2 && game->map[playerY][playerX-1]->playerOnSquare == 0 && game->map[playerY][playerX-1]->bombeOnSquare == 0) {
                    game->map[playerY][playerX]->playerOnSquare = 0;
                    game->map[playerY][playerX-1]->playerOnSquare = 1;
                    game->players[num]->playerSquarePosition.x--;
                    game->players[num]->playerPosition.x -= movingFactorX;
                    game->players[num]->direction = 3;
                    return 4;
                }
            }
        }
    } else if (direction == 5) {
        if ( game->map[playerY][playerX]->bombeOnSquare == 0 ) { //should verify if there in no other bombe
            if (newSeconds >= *seconds+200) {
                if (game->players[num]->playerStats->numberOfBombes) {
                    for(int i = 0; i < game->players[num]->playerStats->numberOfBombesPlanted; i++)
                    {
                        if (game->players[num]->playerBombes->activated[i] == 0 && game->players[num]->playerBombes->exploded[i] == 0) {
                            game->players[num]->playerBombes->activated[i] = 1;
                            game->map[playerY][playerX]->bombeOnSquare = 1;
                            game->players[num]->playerBombes->bombesOnWhichSquare[i][0] = playerY; game->players[num]->playerBombes->bombesOnWhichSquare[i][1] = playerX;
                            game->players[num]->playerStats->numberOfBombes--;
                            game->players[num]->playerBombes->bombeSquarePosition[i].x = game->players[num]->playerSquarePosition.x;
                            game->players[num]->playerBombes->bombeSquarePosition[i].y = game->players[num]->playerSquarePosition.y;

                            game->players[num]->playerBombes->timeOfDetonation[i] = SDL_GetTicks();
                            return 5+i;
                        }
                    }
                }
            }
        }
    }
    return 99;
}

int eventInsideServer() {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 0;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    return 0;
                    break;

                default:
                    printf("Key pressed not recognised : %d\n", e.key.keysym.sym);
                    return 2;
                    break;
            }
        }
    }
    return 1;
}

int playerNumberChangeMonitor(gameStruct *game, int the_socket) {  // for players positions
    char *command = malloc(sizeof(char) * 32);
    memset(command, '\0', 32);
    command[0] = '%';
    for(int i = 0; i < 4; i++) {
        if (game->sockets->clientSockets[i] >= 0) {
            command[i + 1] = 1 + '0';
        } else {
            command[i + 1] = 0 + '0';
        }
    }
    for(int k = 0; k < 4; k++) {
        if (game->players[k]->playerStats->lives <= 0) {
            command[k + 1] = 0 + '0';
        }
    }
    for(int j = 0; j < 4; j++) {
        if (game->sockets->clientSockets[j] >= 0) {
            write(game->sockets->clientSockets[j], command, 16);
        }
    }
    printf("------player number changed : %s\n\n", command);
    char *message = malloc(sizeof(char) * 16);
    memset(command, '\0', 32);
    memset(message, '\0', 16);
    command[0] = '\'';
    int j = 1;
    for(int k = 0; k < 4; k++) {
        if (game->sockets->clientSockets[k] >= 0) {
            command[j] = game->players[k]->playerSquarePosition.x + '0';
            command[j+1] = game->players[k]->playerSquarePosition.y + '0';
            command[j+2] = game->players[k]->playerStats->lives + '0';
            command[j+3] = (game->players[k]->playerStats->speed / 10) + '0';
            command[j+4] = game->players[k]->playerStats->numberOfBombes + '0';
            command[j+5] = game->players[k]->playerStats->numberOfBombesPlanted + '0';
        } else {
            command[j] = 'p';
            command[j+1] = 'p';
            command[j+2] = 'p';
            command[j+3] = 'p';
            command[j+4] = 'p';
            command[j+5] = 'p';
        }
        j+=6;
    }
    printf("%s\n", command);
    // do
    // {
        memset(message, '\0', 16);
        write(the_socket, command, 16);
        read(the_socket, message, 16);
    // } while (message[0] != '\'');
    puts("player number and positions sent");
    return game->sockets->maxConnexions;
}

void bombeTimerServer(gameStruct *game, int num) {
    Uint32 newSeconds = SDL_GetTicks();
    for(int i = 0; i < game->players[num]->playerStats->numberOfBombesPlanted; i++)
    {
        if (newSeconds > game->players[num]->playerBombes->timeOfDetonation[i]+4000 && game->players[num]->playerBombes->activated[i]) {
            bombeExpload(game, num, i);
        }
    }
}

void bombeExpload(gameStruct *game, int num, int bombeNum){
    game->players[num]->playerBombes->activated[bombeNum] = 0;
    game->players[num]->playerBombes->exploded[bombeNum] = 1;

    char *command = malloc(sizeof(char) * 512);
    char *bonusesMessage = malloc(sizeof(char) * 256);
    memset(command, '\0', 512);
    memset(bonusesMessage, '\0', 256);
    command[0] = '\"'; // for explosion
    for(int l = 1; l <= 6; l++)
    {
        command[l] = '0';
    }
    int radiusBombe = game->players[num]->playerStats->radiusOfBombes;
    int goodXplus; int goodXminus; int goodYplus; int goodYminus;
    int bonusSwitch = 1;
    int p = 7;
    for(int i = 0; i < 4; i++) {
        goodXplus = 1; goodXminus = 1; goodYplus = 1; goodYminus = 1;
        for(int j = 1; j <= radiusBombe; j++)
        {
            if (goodYplus) {
                if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.x == bombeX && game->players[i]->playerSquarePosition.y == bombeY+j && game->map[bombeY+j][bombeX]->squareType <= 2) {
                    if (game->players[i]->playerStats->lives) {
                        game->players[i]->playerStats->lives--;
                    }
                    game->map[bombeY+j][bombeX]->playerOnSquare = 0;
                } else if (game->map[bombeY+j][bombeX]->squareType >= 3) {
                    goodYplus = 0;
                } else if (game->map[bombeY+j][bombeX]->squareType == 2 && bonusSwitch <= radiusBombe) {
                    game->map[bombeY+j][bombeX]->squareType = 0;
                    command[p] = bombeY+j + '0'; command[p+1] = bombeX + '0';
                    createBonus(game, bonusesMessage, bombeX, bombeY+j);
                    p+=2;
                }
            }
            if (goodXplus) {
                if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.y == bombeY && game->players[i]->playerSquarePosition.x == bombeX+j && game->map[bombeY][bombeX+j]->squareType <= 2) {
                    if (game->players[i]->playerStats->lives) {
                        game->players[i]->playerStats->lives--;
                    }
                    game->map[bombeY][bombeX+j]->playerOnSquare = 0;
                } else if (game->map[bombeY][bombeX+j]->squareType >= 3) {
                    goodXplus = 0;
                } else if (game->map[bombeY][bombeX+j]->squareType == 2 && bonusSwitch <= radiusBombe) {
                    game->map[bombeY][bombeX+j]->squareType = 0;
                    command[p] = bombeY + '0'; command[p+1] = bombeX+j + '0';
                    createBonus(game, bonusesMessage, bombeX+j, bombeY);
                    p+=2;
                }
            }
            if (goodYminus) {
                if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.x == bombeX && game->players[i]->playerSquarePosition.y == bombeY-j && game->map[bombeY-j][bombeX]->squareType <= 2) {
                    if (game->players[i]->playerStats->lives) {
                        game->players[i]->playerStats->lives--;
                    }
                    game->map[bombeY-j][bombeX]->playerOnSquare = 0;
                } else if (game->map[bombeY-j][bombeX]->squareType >= 3) {
                    goodYminus = 0;
                } else if (game->map[bombeY-j][bombeX]->squareType == 2 && bonusSwitch <= radiusBombe) {
                    game->map[bombeY-j][bombeX]->squareType = 0;
                    command[p] = bombeY-j + '0'; command[p+1] = bombeX + '0';
                    createBonus(game, bonusesMessage, bombeX, bombeY-j);
                    p+=2;
                }
            }
            if (goodXminus) {
                if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.y == bombeY && game->players[i]->playerSquarePosition.x == bombeX-j && game->map[bombeY][bombeX-j]->squareType <= 2) {
                    if (game->players[i]->playerStats->lives) {
                        game->players[i]->playerStats->lives--;
                    }
                    game->map[bombeY][bombeX-j]->playerOnSquare = 0;
                } else if (game->map[bombeY][bombeX-j]->squareType >= 3) {
                    goodXminus = 0;
                } else if (game->map[bombeY][bombeX-j]->squareType == 2 && bonusSwitch <= radiusBombe) {
                    game->map[bombeY][bombeX-j]->squareType = 0;
                    command[p] = bombeY + '0'; command[p+1] = bombeX-j + '0';
                    createBonus(game, bonusesMessage, bombeX-j, bombeY);
                    p+=2;
                }
            }
            bonusSwitch++;
        }
        if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.y == bombeY && game->players[i]->playerSquarePosition.x == bombeX) {
            if (game->players[i]->playerStats->lives) {
                game->players[i]->playerStats->lives--;
            }
        }
        command[i+1] = game->players[i]->playerStats->lives + '0';
    }
    command[5] = num + '0';
    command[6] = bombeNum + '0';
    char *dollarSign = malloc(sizeof(char) * 4);
    memset(dollarSign, '\0', 4);
    dollarSign[0] = '$';
    strcat(command, dollarSign);
    strcat(command, bonusesMessage);
    for(int j = 0; j < 4; j++)
    {
        if (game->sockets->clientSockets[j] >= 0) {
            send(game->sockets->clientSockets[j], command, 256, MSG_NOSIGNAL);
        }
        printf("------server sended : %s\n", command);
    }
}

void bombeContinueExploading(gameStruct *game, int num, int bombeNum){
    int bombTouchedPlayer = 0; // switch used bellow
    char *command = malloc(sizeof(char) * 16);
    memset(command, '\0', 16);
    command[0] = '#'; // for explosion after initial explosion
    for(int i = 0; i < 4; i++) {
        if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.x == bombeX && (game->players[i]->playerSquarePosition.y == bombeY+1 || game->players[i]->playerSquarePosition.y == bombeY-1)) {
            game->players[i]->playerStats->lives--;
            bombTouchedPlayer = 1;
        }
        if (game->players[i]->playerStats->lives && game->players[i]->playerSquarePosition.y == bombeY && (game->players[i]->playerSquarePosition.x == bombeX+1 || game->players[i]->playerSquarePosition.x == bombeX-1)) {
            game->players[i]->playerStats->lives--;
            bombTouchedPlayer = 1;
        }
        command[i+1] = game->players[i]->playerStats->lives + '0';
    }
    command[5] = num + '0';
    command[6] = bombeNum + '0';
    if (bombTouchedPlayer) {
        // SDL_Delay(100);
        for(int j = 0; j < 4; j++)
        {
            if (game->sockets->clientSockets[j] >= 0) {
                send(game->sockets->clientSockets[j], command, 16, MSG_NOSIGNAL);
            }
            printf("------server sended : %s\n", command);
        }
    }
}

void bombeExplosionDuration(gameStruct *game, int num) {
    Uint32 newSeconds = SDL_GetTicks();
    for(int i = 0; i < game->players[num]->playerStats->numberOfBombesPlanted; i++)
    {
        if (newSeconds > game->players[num]->playerBombes->timeOfDetonation[i]+4000 && newSeconds < game->players[num]->playerBombes->timeOfDetonation[i]+5000 && game->players[num]->playerBombes->activated[i]) {
            bombeExpload(game, num, i);
            game->map[game->players[num]->playerBombes->bombesOnWhichSquare[i][0]][game->players[num]->playerBombes->bombesOnWhichSquare[i][1]]->bombeOnSquare = 0;
        } else if (newSeconds > game->players[num]->playerBombes->timeOfDetonation[i]+4000 && newSeconds < game->players[num]->playerBombes->timeOfDetonation[i]+5000 && game->players[num]->playerBombes->exploded[i]) {
            bombeContinueExploading(game, num, i);
        } else if (game->players[num]->playerBombes->exploded[i]) {
            game->players[num]->playerBombes->exploded[i] = 0;
        }
    }
}

int createBonus(gameStruct *game, char *message, int positionX, int positionY){
    int random = rand();
    char *bonusesMessageNew = malloc(sizeof(char) * 8);
    memset(bonusesMessageNew, '\0', 8);
    int randomCoonf = random%30;
    if (randomCoonf <= 18) {
        if (randomCoonf >= 0 && randomCoonf <= 7) {
            randomCoonf = 0;
        } else if (randomCoonf <= 11) {
            randomCoonf = 1;
        } else if (randomCoonf <= 14) {
            randomCoonf = 2;
        } else if (randomCoonf <= 15) {
            randomCoonf = 3;
        } else if (randomCoonf <= 18) {
            randomCoonf = 4;
        }
        for(int i = 0; i < 16; i++)
        {
            if (game->bonuses->bonusesArray[i]->bonusType == -1) {
                game->bonuses->bonusesArray[i]->bonusType = randomCoonf;
                game->bonuses->bonusesArray[i]->bonusSquarePosition.x = positionX;
                game->bonuses->bonusesArray[i]->bonusSquarePosition.y = positionY;
                game->bonuses->bonusesArray[i]->bonusSpritePosition.x = 0; game->bonuses->bonusesArray[i]->bonusSpritePosition.y = 0;
                game->bonuses->bonusesArray[i]->bonusSpritePosition.h = 64; game->bonuses->bonusesArray[i]->bonusSpritePosition.w = 64;
                bonusesMessageNew[0] = i + '0';
                bonusesMessageNew[1] = randomCoonf + '0';
                bonusesMessageNew[2] = positionX + '0';
                bonusesMessageNew[3] = positionY + '0';
                game->map[positionY][positionX]->bonusOnSquare = i;
                printf("the i is : %d\n", i);
                i = 16;
            }
        }
        strcat(message, bonusesMessageNew);
    }
}

int bonusTaking(gameStruct *game, int num, char *message){
    if (game->map[playerY][playerX]->bonusOnSquare != -1) {
        int bonusNumber = game->bonuses->bonusesArray[game->map[playerY][playerX]->bonusOnSquare]->bonusType;
        message[3] = game->map[playerY][playerX]->bonusOnSquare + '0';
        message[4] = game->bonuses->bonusesArray[game->map[playerY][playerX]->bonusOnSquare]->bonusType + '0';
        game->bonuses->bonusesArray[game->map[playerY][playerX]->bonusOnSquare]->bonusType = -1;
        if (bonusNumber == 0) {
            if (game->players[num]->playerStats->numberOfBombes >= 15) {
                game->players[num]->playerStats->numberOfBombes = 20;
            } else {
                game->players[num]->playerStats->numberOfBombes+=5;
            }
            message[5] = game->players[num]->playerStats->numberOfBombes + '0';
        } else if (bonusNumber == 1) {
            if (game->players[num]->playerStats->speed > 150) {
                game->players[num]->playerStats->speed -= 50;
            }
            message[5] = (game->players[num]->playerStats->speed / 50) + '0';
        } else if (bonusNumber == 2) {
            if (game->players[num]->playerStats->radiusOfBombes < 9) {
                game->players[num]->playerStats->radiusOfBombes+=2;
            }
            message[5] = game->players[num]->playerStats->radiusOfBombes + '0';
        } else if (bonusNumber == 3) {
            if (game->players[num]->playerStats->lives < 2) {
                game->players[num]->playerStats->lives++;
            }
            message[5] = game->players[num]->playerStats->lives + '0';
        } else if (bonusNumber == 4) {
            if (game->players[num]->playerStats->numberOfBombesPlanted < 4) {
                game->players[num]->playerStats->numberOfBombesPlanted++;
            }
            message[5] = game->players[num]->playerStats->numberOfBombesPlanted + '0';
        }
        game->map[playerY][playerX]->bonusOnSquare = -1;
    }
}

void mapToMessage(gameStruct *game, char *message){
    message[0] = '(';
    int k = 1; // variable pour marché 3 cases a la fois
    for(int i = 0; i < game->numberOfBlocks_Y; i++)
    {
        for(int j = 0; j < game->numberOfBlocks_X; j++)
        {
            if (game->map[i][j]->squareType == 2) {
                message[k] = 3 + '0';
                message[k+1]= i + '0';
                message[k+2] = j + '0';
                k+=3;
            }
        }
    }
}

int bonusToMessage(gameStruct *game, char *message){
    message[0] = ')';
    int k = 1; // variable pour marché 3 cases a la fois
    for(int i = 0; i < 16; i++)
    {
        if (game->bonuses->bonusesArray[i]->bonusType != -1) {
            message[k] = i + '0';
            message[k+1] = game->bonuses->bonusesArray[i]->bonusType + '0';
            message[k+2] = game->bonuses->bonusesArray[i]->bonusSquarePosition.x + '0';
            message[k+3] = game->bonuses->bonusesArray[i]->bonusSquarePosition.y + '0';
            k += 4;
        }
    }
}

int playerNumberToMessage(gameStruct *game, char *message){
    message[0] = '%';
    for(int i = 0; i < 4; i++) {
        if (game->sockets->clientSockets[i] >= 0) {
            message[i + 1] = 1 + '0';
        } else {
            message[i + 1] = 0 + '0';
        }
    }
    message[5] = '\'';
    int j = 6;
    for(int k = 0; k < 4; k++) {
        if (game->sockets->clientSockets[k] >= 0) {
            message[j] = game->players[k]->playerSquarePosition.x + '0';
            message[j+1] = game->players[k]->playerSquarePosition.y + '0';
            message[j+2] = game->players[k]->playerStats->lives + '0';
            message[j+3] = (game->players[k]->playerStats->speed / 10) + '0';
            message[j+4] = game->players[k]->playerStats->numberOfBombes + '0';
            message[j+5] = game->players[k]->playerStats->numberOfBombesPlanted + '0';
        } else {
            message[j] = 'p';
            message[j+1] = 'p';
            message[j+2] = 'p';
            message[j+3] = 'p';
            message[j+4] = 'p';
            message[j+5] = 'p';
        }
        j+=6;
    }
}

int playerStatsToMessage(gameStruct *game, char *message){
    message[0] = '*';
    int j = 1;
    for(int i = 0; i < 4; i++)
    {
        message[j] = i + '0';
        message[j+1] = game->players[i]->playerStats->lives + '0';
        message[j+2] = (game->players[i]->playerStats->speed / 50) + '0';
        message[j+3] = game->players[i]->playerStats->numberOfBombes + '0';
        message[j+4] = game->players[i]->playerStats->numberOfBombesPlanted + '0';
        message[j+5] = game->players[i]->playerStats->radiusOfBombes + '0';
        j+=6;
    }
}

int concatenateGameStat(gameStruct *game, char *message){
    char *messageMap = malloc(sizeof(char) * 255);
    char *messageBonuses = malloc(sizeof(char) * 128);
    char *messagePlayers = malloc(sizeof(char) * 64);
    char *messageStats = malloc(sizeof(char) * 64);
    memset(messageMap, '\0', 255);
    memset(messageBonuses, '\0', 128);
    memset(messagePlayers, '\0', 64);
    memset(messageStats, '\0', 64);
    mapToMessage(game, messageMap);
    bonusToMessage(game, messageBonuses);
    playerNumberToMessage(game, messagePlayers);
    playerStatsToMessage(game, messageStats);
    strcat(messageMap, messageBonuses);
    strcat(messageMap, messagePlayers);
    strcat(messageMap, messageStats);
    strcat(message, messageMap);
}
