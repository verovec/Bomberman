#ifndef GAME_H
#define GAME_H

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

typedef struct {
    SDL_Rect hudPosition;
    SDL_Rect playerNumberPosition;
    SDL_Rect alivePosition;
    SDL_Rect numberOfBombesPosition;
    SDL_Rect numberOfBombesPlantedPosition;
    SDL_Rect radiusOfBombesPosition;
    SDL_Rect speedPosition;

    SDL_Texture *hudTexture;
    SDL_Texture *hudTextureDead;
    SDL_Texture *playerNumberTexture;
    SDL_Texture *aliveTexture;
    SDL_Texture *numberOfBombesTexture;
    SDL_Texture *numberOfBombesPlantedTexture;
    SDL_Texture *radiusOfBombesTexture;
    SDL_Texture *speedTexture;
}hudStruct;

typedef struct {
    SDL_Rect bonusPosition;
    SDL_Point bonusSquarePosition;
    SDL_Rect bonusSpritePosition;
    int bonusType;
}bonus;

typedef struct {
    SDL_Texture *bonusTexture[5];
    bonus **bonusesArray;
}gameBonusGeneral;

typedef struct {
    SDL_Texture *mapTextures[5];
    int mapNumber;
}mapGeneral;

typedef struct {
    // SDL_Texture *squareTexture;
    SDL_Rect squarePosition;
    int squareType;

    int bombeOnSquare;
    int playerOnSquare;
    int bonusOnSquare;
}caseMap;

typedef struct {
    char *port;
    char *ipAdresse;

    int portInt;
    int ipAdresseInt;
}serverInfos;

typedef struct {
    int lives;
    int speed;
    int numberOfBombes;
    int numberOfBombesPlanted;
    int radiusOfBombes;
}gameStatsPlayer;

typedef struct {
    SDL_Texture *bombeTexture;
    SDL_Texture *flameTexture[9];
}gameBombeGeneral;

typedef struct {
    SDL_Rect bombePosition[4];
    SDL_Rect bombeSpritePosition[4];
    SDL_Point bombeSquarePosition[4];
    int activated[4];
    int exploded[4];
    int bombesOnWhichSquare[4][2];
    Uint32 timeOfDetonation[4];
}gameBombePlayer;

typedef struct {
    SDL_Texture *playerTexture[6];
    SDL_Rect playerPosition;
    SDL_Rect playerSpritePosition;
    SDL_Point playerSquarePosition;

    gameBombePlayer *playerBombes;
    gameStatsPlayer *playerStats;
    int direction;
}gamePlayer;

typedef struct {
    int clientSockets[4];
    int currentNumOfSocket;
    int maxConnexions;
}gameSockets;

typedef struct {
    TTF_Font *font;
    TTF_Font *font_12;
    SDL_Surface *inputFont;
    SDL_Surface *backgroundSurface;
    SDL_Texture *inputTexture;
    SDL_Texture *backgroundTexture;
    SDL_Texture *backgroundTexture_selectMap;
    SDL_Texture *backgroundTexture_serverRunning;

    SDL_Rect inputPosition;
    int menuSelected;
    int inputSelected;

    SDL_Rect errorPosition[2];
    SDL_Rect errorSpritePosition[2];
    SDL_Texture *errorTexture[2];

    SDL_Rect firstButtonPosition;
    SDL_Rect firstButtonSpritePosition;
    SDL_Texture *firstButtonTexture;

    SDL_Rect secondButtonPosition;
    SDL_Rect secondButtonSpritePosition;
    SDL_Texture *secondButtonTexture;

    SDL_Rect selectPortPosition;
    SDL_Texture *selectPortTexture;

    SDL_Rect selectIpPosition;
    SDL_Texture *selectIpTexture;

    SDL_Rect selectInputPosition;
    SDL_Texture *selectInputTexture;

    SDL_Rect mapSelectionPosition[3];
    SDL_Rect mapSelectionSpritePosition[3];
    SDL_Texture *mapSelectionTexture[3];
}gameMenu;

typedef struct {
    SDL_Point screenSize;
    SDL_Point screenSize_withHUD;
    SDL_Rect screenSize_all;
    SDL_Window *window;
    SDL_Renderer *renderer;
    hudStruct *hud[4];

    // SDL_Texture *mapTexture;

    gamePlayer *players[4];
    gameBombeGeneral *bombe;
    gameMenu *menu;
    serverInfos *infos;

    int numberOfBlocks_X;
    int numberOfBlocks_Y;
    caseMap ***map;
    mapGeneral *mapCaracteristics;
    gameSockets *sockets;
    gameBonusGeneral *bonuses;

    SDL_Texture *backgroundTexture_hud[3];
    SDL_Rect backgroundPosition_hud;
}gameStruct;

// don't know if i need this
//
// typedef struct
// {
//     int playerNumber;
//     int mouvement;
// } serverMessageStruct;


gameStruct* gameInit();

void gameDestroy(gameStruct *game);
void gameDraw(gameStruct *game, int sizeX, int sizeY, int playersThatAreHere[4]);
void menuDraw(gameStruct *game, Uint32 *seconds);
//int gameEvent(gameStruct *game, Uint32 *seconds, int playerNumber); //secondes pour attacher les mouvements au temps et non pas au performances
int menuEvent(gameStruct *game);
int menuTextInputEvent(char *port, gameStruct *game, int size);

int gameMovePlayer(gameStruct *game, SDL_Keycode direction, Uint32 *seconds, int num);
void generateBombe(gameStruct *game, int num);

void *connectionHandler(void *socket);
void *theServer(void *gameVoid);
int theClient(char *ip, int port, gameStruct *game);
void changeText(gameStruct *game, char *message, int size, SDL_Rect rect, SDL_Surface *surface, SDL_Texture *texture);
//void changeFont(gameStruct *game, SDL_Surface *buttonFont, SDL_Texture *buttonTexture, int size, char *message);

SDL_Texture *changeText_(gameStruct *game, char *message, int size);
SDL_Texture *changeText_fixedSize(gameStruct *game, char *message);

// structures de la map
gameStruct *mapInit(int sizeX, int sizeY, gameStruct *game);
gameStruct *playersInit(gameStruct *game);
gameStruct *socketsInit(gameStruct *game);


// events_client
int gameEventClient_sendMessage(gameStruct *game, Uint32 *seconds, int playerNumber, char *message, int playersonline);
int gameMovePlayer_client(gameStruct *game, SDL_Keycode direction, Uint32 *seconds, int num, int speacialCommand);
int gameEventClient_recieveMessage(gameStruct *game, char *message, int playersThatAreHere[4], Uint32 *seconds, int movementNumber, int *playerToMoveNumber, int *direction, int *speacialCommand, int the_socket);
gameStruct *mapInitClient(int sizeX, int sizeY, gameStruct *game);

// events_server
int eventServer (gameStruct *game, int direction, Uint32 *seconds, int num);
int gameMovePlayer_server(gameStruct *game, int direction, Uint32 *seconds, int num);
int eventInsideServer();
int playerNumberChangeMonitor(gameStruct *game, int the_socket);
void bombeTimerServer(gameStruct *game, int num);
void bombeExpload(gameStruct *game, int num, int bombeNum);
void bombeContinueExploading(gameStruct *game, int num, int bombeNum);
void bombeExplosionDuration(gameStruct *game, int num);
gameStruct *mapInitServer(int sizeX, int sizeY, gameStruct *game);

int animations(gameStruct *game, int animationsArray[][4], Uint32 secondsAnimations[]);
int animationHandling(gameStruct *game, int animationsArrayElement[3], Uint32 secondsAnimations[], int playerNumber, int elementNumber, int speacialCommand);
void initSurfaces(SDL_Surface *surfacesPlayers[4][6]);
void mapToMessage(gameStruct *game, char *message);
gameStruct *initBonuses(int sizeX, int sizeY, gameStruct *game);
int createBonus(gameStruct *game, char *message, int positionX, int positionY);
gameStruct *buttonsInit(gameStruct *game);
SDL_Rect animationMenu(SDL_Rect spritePosition,  Uint32 *seconds, int maxSpritePosition, int incrementSpritePosition);
int bonusTaking(gameStruct *game, int num, char *message);
int bonusTaking_Client(gameStruct *game, int num, char *message);
int bonusAnimations(gameStruct *game, Uint32 *seconds);
int bonusToMessage(gameStruct *game, char *message);
void menuDraw_serverRunning(gameStruct *game);
gameStruct *initHud(gameStruct *game);
int concatenateGameStat(gameStruct *game, char *message);
SDL_Texture *changeText_fixedSize_12(gameStruct *game, char *message);
int playerStatsToMessage(gameStruct *game, char *message);

#endif
