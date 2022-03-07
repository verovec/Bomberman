#include "../headers/main.h"

/* INIT THE GAME ///////////////////////////////////////////////////*/
gameStruct *gameInit(){

// init and malloc the game variables
    gameStruct *game = NULL;
    game = malloc(sizeof(gameStruct));
    game->bombe = malloc(sizeof(gameBombeGeneral));
    game->menu = malloc(sizeof(gameMenu));
    game->infos = malloc(sizeof(serverInfos));
    game->sockets = malloc(sizeof(gameSockets));
    game->mapCaracteristics = malloc(sizeof(mapGeneral));


// malloc the players
    for(int i = 0; i < 4; i++)
    {
        game->players[i] = malloc(sizeof(gamePlayer));
        game->players[i]->playerBombes = malloc(sizeof(gameBombePlayer));
        game->players[i]->playerStats = malloc(sizeof(gameStatsPlayer));
    }


// nombre de blocks dans la map [n]x[n]
    game->numberOfBlocks_X = 15;
    game->numberOfBlocks_Y = 11;

    game->sockets->maxConnexions = 0;

    if (!game) {
        printf("Error : Game structure Initialisation");
        gameDestroy(game);
        return NULL;
    }

// init the sdl and sdl_ttf
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error : SDL Initialisation : %s \n",SDL_GetError());
		gameDestroy(game);
        return NULL;
    }
    printf("SDL initialised correctly\n");
    if (TTF_Init() != 0) {
        printf("Error : SDL_TTF Initialisation : %s \n",TTF_GetError());
		gameDestroy(game);
        return NULL;
    }
    printf("SDL_TTF initialised correctly\n");

    game->screenSize.x = 720;
    game->screenSize.y = 528;
    game->screenSize_withHUD.x = 940;
    game->screenSize_withHUD.y = 528;
    game->screenSize_all.w = 940;
    game->screenSize_all.h = 528;
    game->screenSize_all.x = 0;
    game->screenSize_all.y = 0;
    game->window = NULL;
    game->renderer = NULL;

    game->bombe->bombeTexture = NULL;

    game->menu->menuSelected = 1;
    game->menu->inputFont = NULL;
    game->menu->font = TTF_OpenFont("./fonts/font.ttf", 20);
    game->menu->font_12 = TTF_OpenFont("./fonts/font.ttf", 12);

    game->menu->inputSelected = 0;
    game->menu->inputPosition.x = game->screenSize.x / 32;
    game->menu->inputPosition.y = game->screenSize.x / 12;
    game->menu->inputPosition.w = game->screenSize.x / 16;
    game->menu->inputPosition.h = game->screenSize.y / 16;


// port, ip an other network stuf
    game->infos->port = malloc(sizeof(char) * 8);
    game->infos->ipAdresse = malloc(sizeof(char) * 24);
    memset(game->infos->port, '\0', 8);
    memset(game->infos->ipAdresse, '\0', 24);
    game->infos->port[0] = ':';
    game->infos->ipAdresse[0] = ':';

// create the window and renderer
    game->window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    game->screenSize_withHUD.x, game->screenSize_withHUD.y, SDL_WINDOW_SHOWN);

    if (game->window) {
        game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
        if (!game->renderer) {
            printf("Error : SDL Renderer creation : %s \n",SDL_GetError());
            gameDestroy(game);
            return NULL;
        }
        printf("SDL Window and Renderer initialised correctly\n");
    } else {
        printf("Error : SDL Window creation : %s \n",SDL_GetError());
        gameDestroy(game);
        return NULL;
    }

// init the 4 players  // init and malloc the map
    game = playersInit(game);
    game = mapInitClient(game->numberOfBlocks_X, game->numberOfBlocks_Y, game);
    game = socketsInit(game);
    game = initBonuses(game->numberOfBlocks_X, game->numberOfBlocks_Y, game);
    game = buttonsInit(game);
    game = initHud(game);
    //printf("%d", game->map[0][0]->squarePosition.y);

    SDL_Surface *surfaceBombe = IMG_Load("./images/bombe.png");
    if (!surfaceBombe) {
        printf("Error : SDL Surface creation (image : \"bombe.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->bombe->bombeTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceBombe);
        if (!game->bombe->bombeTexture) {
            printf("Error : SDL Texture creation bombe: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceBombe);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBombe);
        printf("bombe texture initialised correctly\n");
    }

    SDL_Surface *surfaceFlame_center = IMG_Load("./images/flames/flames_center.png");
    SDL_Surface *surfaceFlame_up = IMG_Load("./images/flames/flames_up.png");
    SDL_Surface *surfaceFlame_down = IMG_Load("./images/flames/flames_down.png");
    SDL_Surface *surfaceFlame_right = IMG_Load("./images/flames/flames_right.png");
    SDL_Surface *surfaceFlame_left = IMG_Load("./images/flames/flames_left.png");
    SDL_Surface *surfaceFlame_up_center = IMG_Load("./images/flames/flames_up_center.png");
    SDL_Surface *surfaceFlame_down_center = IMG_Load("./images/flames/flames_down_center.png");
    SDL_Surface *surfaceFlame_right_center = IMG_Load("./images/flames/flames_right_center.png");
    SDL_Surface *surfaceFlame_left_center = IMG_Load("./images/flames/flames_left_center.png");
    if (!surfaceFlame_up || !surfaceFlame_down || !surfaceFlame_right || !surfaceFlame_left) {
        printf("Error : SDL Surface creation (image : \"flames_'up/down/left/right'.png\"): %s \n",IMG_GetError());
        if (surfaceFlame_center) {
            SDL_FreeSurface(surfaceFlame_center);
        }
        if (surfaceFlame_up) {
            SDL_FreeSurface(surfaceFlame_up);
        }
        if (surfaceFlame_down) {
            SDL_FreeSurface(surfaceFlame_down);
        }
        if (surfaceFlame_right) {
            SDL_FreeSurface(surfaceFlame_right);
        }
        if (surfaceFlame_left) {
            SDL_FreeSurface(surfaceFlame_left);
        }
        if (surfaceFlame_up) {
            SDL_FreeSurface(surfaceFlame_up_center);
        }
        if (surfaceFlame_down) {
            SDL_FreeSurface(surfaceFlame_down_center);
        }
        if (surfaceFlame_right) {
            SDL_FreeSurface(surfaceFlame_right_center);
        }
        if (surfaceFlame_left) {
            SDL_FreeSurface(surfaceFlame_left_center);
        }
        gameDestroy(game);
        return NULL;
    } else {
        game->bombe->flameTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_center);
        game->bombe->flameTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_up);
        game->bombe->flameTexture[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_down);
        game->bombe->flameTexture[3] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_right);
        game->bombe->flameTexture[4] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_left);
        game->bombe->flameTexture[5] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_up_center);
        game->bombe->flameTexture[6] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_down_center);
        game->bombe->flameTexture[7] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_right_center);
        game->bombe->flameTexture[8] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_left_center);
        for(int i = 0; i < 9; i++)
        {
            if (!game->bombe->flameTexture[i]) {
                printf("Error : SDL Texture creation flame: %s \n",IMG_GetError());
                SDL_FreeSurface(surfaceFlame_center); SDL_FreeSurface(surfaceFlame_up); SDL_FreeSurface(surfaceFlame_down); SDL_FreeSurface(surfaceFlame_right); SDL_FreeSurface(surfaceFlame_left); SDL_FreeSurface(surfaceFlame_up_center); SDL_FreeSurface(surfaceFlame_down_center); SDL_FreeSurface(surfaceFlame_right_center); SDL_FreeSurface(surfaceFlame_left_center);
                gameDestroy(game);
                return NULL;
            }
        }
        SDL_FreeSurface(surfaceFlame_center); SDL_FreeSurface(surfaceFlame_up); SDL_FreeSurface(surfaceFlame_down); SDL_FreeSurface(surfaceFlame_right); SDL_FreeSurface(surfaceFlame_left); SDL_FreeSurface(surfaceFlame_up_center); SDL_FreeSurface(surfaceFlame_down_center); SDL_FreeSurface(surfaceFlame_right_center); SDL_FreeSurface(surfaceFlame_left_center);
        printf("flame texture initialised correctly\n");
    }

    SDL_Surface *surfaceBackground = IMG_Load("./images/background.png");
    if (!surfaceBackground) {
        printf("Error : SDL Surface creation (image : \"background.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->backgroundTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceBackground);
        if (!game->menu->backgroundTexture) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceBackground);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBackground);
        printf("input texture initialised correctly\n");
    }

    SDL_Surface *surfaceBackground_serverRunning = IMG_Load("./images/background_serverRunning.png");
    if (!surfaceBackground_serverRunning) {
        printf("Error : SDL Surface creation (image : \"background_serverRunning.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->backgroundTexture_serverRunning = SDL_CreateTextureFromSurface(game->renderer, surfaceBackground_serverRunning);
        if (!game->menu->backgroundTexture_serverRunning) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceBackground_serverRunning);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBackground_serverRunning);
        printf("input texture initialised correctly\n");
    }
    printf("menu texture initialised correctly\n\n");
    return game;
}

/* INIT THE MAP ///////////////////////////////////////////////////*/
gameStruct *mapInit(int sizeX, int sizeY, gameStruct *game){
    game->map = (caseMap***)malloc(sizeY * sizeof(caseMap **));
    for(int i = 0; i < sizeY; i++)
    {
        game->map[i] = (caseMap**)malloc(sizeX * sizeof(caseMap *));
    }
    SDL_Surface *surfaceCase_map1_1 = IMG_Load("./images/map/0/green_1.png"); SDL_Surface *surfaceCase_map1_2 = IMG_Load("./images/map/0/green_2.png");
    SDL_Surface *surfaceCase_map1_3 = IMG_Load("./images/map/0/destructible.png"); SDL_Surface *surfaceCase_map1_4 = IMG_Load("./images/map/0/solid.png");
    SDL_Surface *surfaceCase_map1_5 = IMG_Load("./images/map/0/solid_exterior.png");
    if (!surfaceCase_map1_1 || !surfaceCase_map1_2 || !surfaceCase_map1_3 || !surfaceCase_map1_4 || !surfaceCase_map1_5) {
        printf("Error : SDL Surface creation (image : \"green_1.png\" or \"green_2.png\" or \"solid.png\" or \"solid_exterior.png\" or \"destructible.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    }
    game->mapCaracteristics->mapNumber = 0;
    game->mapCaracteristics->mapTextures[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_1);
    game->mapCaracteristics->mapTextures[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_2);
    game->mapCaracteristics->mapTextures[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_3);
    game->mapCaracteristics->mapTextures[3] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_4);
    game->mapCaracteristics->mapTextures[4] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_5);
    for(int j = 0; j < sizeY; j++)
    {
        for(int k = 0; k < sizeX; k++)
        {
            game->map[j][k] = malloc(sizeof(caseMap));
            int random = rand();
            if (k == 0 || j == 0 || k == sizeX-1 || j == sizeY-1) { // case pourtoure innaccessibles
                game->map[j][k]->squareType = 4;
            } else if (j%2 == 1 && k%2 == 1) { // case simples_2
                if (random%4 == 0 || (k == 1 && j == 1) || (k == sizeX-2 && j == sizeY-2) || (k == 1 && j == sizeY-2) || (k == sizeX-2 && j == 1)) {
                    game->map[j][k]->squareType = 1;
                } else {
                    game->map[j][k]->squareType = 2;
                }
            } else if (j%2 == 1 || k%2 == 1) { // case simples
                if (random%4 == 0 || (k == 2 && j == 1) || (k == 1 && j == 2) || (k == sizeX-3 && j == sizeY-2) || (k == sizeX-2 && j == sizeY-3) || (k == 2 && j == sizeY-2) || (k == 1 && j == sizeY-3) || (k == sizeX-3 && j == 1) || (k == sizeX-2 && j == 2)) {
                    game->map[j][k]->squareType = 0;
                } else {
                    game->map[j][k]->squareType = 2;
                }
            } else { // case innaccessibles
                game->map[j][k]->squareType = 3;
            }
            game->map[j][k]->playerOnSquare = 0;
            game->map[j][k]->bombeOnSquare = 0;
            game->map[j][k]->bonusOnSquare = -1;
        // taille de la case
            game->map[j][k]->squarePosition.w = game->screenSize.x / sizeX;
            game->map[j][k]->squarePosition.h = game->screenSize.y / sizeY;

        // position (pixels) de la case
            if (j == 0) {
                game->map[j][k]->squarePosition.y = 0;
            } else {
                game->map[j][k]->squarePosition.y = game->map[j - 1][k]->squarePosition.y + game->map[j][k]->squarePosition.h;
            }
            if (k == 0) {
                game->map[j][k]->squarePosition.x = 0;
            } else {
                game->map[j][k]->squarePosition.x = game->map[j][k - 1]->squarePosition.x + game->map[j][k]->squarePosition.w;
            }
        }
    }
    SDL_FreeSurface(surfaceCase_map1_1); SDL_FreeSurface(surfaceCase_map1_2); SDL_FreeSurface(surfaceCase_map1_3); SDL_FreeSurface(surfaceCase_map1_4); SDL_FreeSurface(surfaceCase_map1_5);
    printf("Map textures initialised correctly\n");
    return game;
}

// map init client
gameStruct *mapInitClient(int sizeX, int sizeY, gameStruct *game){
    game->map = (caseMap***)malloc(sizeY * sizeof(caseMap **));
    for(int i = 0; i < sizeY; i++)
    {
        game->map[i] = (caseMap**)malloc(sizeX * sizeof(caseMap *));
    }
    SDL_Surface *surfaceCase_map1_1 = IMG_Load("./images/map/0/green_1.png"); SDL_Surface *surfaceCase_map1_2 = IMG_Load("./images/map/0/green_2.png");
    SDL_Surface *surfaceCase_map1_3 = IMG_Load("./images/map/0/destructible.png"); SDL_Surface *surfaceCase_map1_4 = IMG_Load("./images/map/0/solid.png");
    SDL_Surface *surfaceCase_map1_5 = IMG_Load("./images/map/0/solid_exterior.png");
    if (!surfaceCase_map1_1 || !surfaceCase_map1_2 || !surfaceCase_map1_3 || !surfaceCase_map1_4 || !surfaceCase_map1_5) {
        printf("Error : SDL Surface creation (image : \"green_1.png\" or \"green_2.png\" or \"solid.png\" or \"solid_exterior.png\" or \"destructible.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    }
    game->mapCaracteristics->mapNumber = 0;
    game->mapCaracteristics->mapTextures[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_1);
    game->mapCaracteristics->mapTextures[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_2);
    game->mapCaracteristics->mapTextures[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_3);
    game->mapCaracteristics->mapTextures[3] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_4);
    game->mapCaracteristics->mapTextures[4] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_5);
    for(int j = 0; j < sizeY; j++)
    {
        for(int k = 0; k < sizeX; k++)
        {
            game->map[j][k] = malloc(sizeof(caseMap));
            if (k == 0 || j == 0 || k == sizeX-1 || j == sizeY-1) { // case pourtoure innaccessibles
                game->map[j][k]->squareType = 4;
            } else if (j%2 == 1 && k%2 == 1) { // case simples_2
                game->map[j][k]->squareType = 1;
            } else if (j%2 == 1 || k%2 == 1) { // case simples
                game->map[j][k]->squareType = 0;
            } else { // case innaccessibles
                game->map[j][k]->squareType = 3;
            }
            game->map[j][k]->playerOnSquare = 0;
            game->map[j][k]->bombeOnSquare = 0;
            game->map[j][k]->bonusOnSquare = -1;
        // taille de la case
            game->map[j][k]->squarePosition.w = game->screenSize.x / sizeX;
            game->map[j][k]->squarePosition.h = game->screenSize.y / sizeY;

        // position de la case
            if (j == 0) {
                game->map[j][k]->squarePosition.y = 0;
            } else {
                game->map[j][k]->squarePosition.y = game->map[j - 1][k]->squarePosition.y + game->map[j][k]->squarePosition.h;
            }
            if (k == 0) {
                game->map[j][k]->squarePosition.x = 0;
            } else {
                game->map[j][k]->squarePosition.x = game->map[j][k - 1]->squarePosition.x + game->map[j][k]->squarePosition.w;
            }
        }
    }
    SDL_FreeSurface(surfaceCase_map1_1); SDL_FreeSurface(surfaceCase_map1_2); SDL_FreeSurface(surfaceCase_map1_3); SDL_FreeSurface(surfaceCase_map1_4); SDL_FreeSurface(surfaceCase_map1_5);
    printf("Map textures initialised correctly\n");
    return game;
}

gameStruct *mapInitServer(int sizeX, int sizeY, gameStruct *game){
    for(int j = 0; j < sizeY; j++)
    {
        for(int k = 0; k < sizeX; k++)
        {
            int random = rand();
            if (k == 0 || j == 0 || k == sizeX-1 || j == sizeY-1) { // case pourtoure innaccessibles
                game->map[j][k]->squareType = 4;
            } else if (j%2 == 1 && k%2 == 1) { // case simples_2
                if (random%4 == 0 || (k == 1 && j == 1) || (k == sizeX-2 && j == sizeY-2) || (k == 1 && j == sizeY-2) || (k == sizeX-2 && j == 1)) {
                    game->map[j][k]->squareType = 1;
                } else {
                    game->map[j][k]->squareType = 2;
                }
            } else if (j%2 == 1 || k%2 == 1) { // case simples
                if (random%4 == 0 || (k == 2 && j == 1) || (k == 1 && j == 2) || (k == sizeX-3 && j == sizeY-2) || (k == sizeX-2 && j == sizeY-3) || (k == 2 && j == sizeY-2) || (k == 1 && j == sizeY-3) || (k == sizeX-3 && j == 1) || (k == sizeX-2 && j == 2)) {
                    game->map[j][k]->squareType = 0;
                } else {
                    game->map[j][k]->squareType = 2;
                }
            } else { // case innaccessibles
                game->map[j][k]->squareType = 3;
            }
        }
    }
    printf("Map textures initialised correctly\n");
    return game;
}

/* INIT THE 4 PLAYERS ///////////////////////////////////////////////////*/
gameStruct *playersInit(gameStruct *game){
// player 1
    // game->players[0]->playerTexture = NULL;
    game->players[0]->playerPosition.x = 48;
    game->players[0]->playerPosition.y = 30;
    game->players[0]->playerPosition.w = game->screenSize.x / 15;
    game->players[0]->playerPosition.h = game->screenSize.y / 10;

    game->players[0]->playerSpritePosition.x = 0;
    game->players[0]->playerSpritePosition.y = 0;
    game->players[0]->playerSpritePosition.w = 67;
    game->players[0]->playerSpritePosition.h = 108;

    game->players[0]->playerSquarePosition.x = 1;
    game->players[0]->playerSquarePosition.y = 1;

// player 2
    // game->players[1]->playerTexture = NULL;
    game->players[1]->playerPosition.x = 48 + ((game->numberOfBlocks_X-3) * 48);
    game->players[1]->playerPosition.y = 30 + ((game->numberOfBlocks_Y-3) * 48);
    game->players[1]->playerPosition.w = game->screenSize.x / 15;
    game->players[1]->playerPosition.h = game->screenSize.y / 10;

    game->players[1]->playerSpritePosition.x = 0;
    game->players[1]->playerSpritePosition.y = 0;
    game->players[1]->playerSpritePosition.w = 67;
    game->players[1]->playerSpritePosition.h = 108;

    game->players[1]->playerSquarePosition.x = game->numberOfBlocks_X-2;
    game->players[1]->playerSquarePosition.y = game->numberOfBlocks_Y-2;

// player 3
    // game->players[2]->playerTexture = NULL;
    game->players[2]->playerPosition.x = 48 + ((game->numberOfBlocks_X-3) * 48);
    game->players[2]->playerPosition.y = 30;
    game->players[2]->playerPosition.w = game->screenSize.x / 15;
    game->players[2]->playerPosition.h = game->screenSize.y / 10;

    game->players[2]->playerSpritePosition.x = 0;
    game->players[2]->playerSpritePosition.y = 0;
    game->players[2]->playerSpritePosition.w = 67;
    game->players[2]->playerSpritePosition.h = 108;

    game->players[2]->playerSquarePosition.x = game->numberOfBlocks_X-2;
    game->players[2]->playerSquarePosition.y = 1;

// player 4
    // game->players[3]->playerTexture = NULL;
    game->players[3]->playerPosition.x = 48;
    game->players[3]->playerPosition.y = 30 + ((game->numberOfBlocks_Y-3) * 48);
    game->players[3]->playerPosition.w = game->screenSize.x / 15;
    game->players[3]->playerPosition.h = game->screenSize.y / 10;

    game->players[3]->playerSpritePosition.x = 0;
    game->players[3]->playerSpritePosition.y = 0;
    game->players[3]->playerSpritePosition.w = 67;
    game->players[3]->playerSpritePosition.h = 108;

    game->players[3]->playerSquarePosition.x = 1;
    game->players[3]->playerSquarePosition.y = game->numberOfBlocks_Y-2;

    for(int i = 0; i < 4; i++)
    {
        game->players[i]->playerStats->radiusOfBombes = 1;
        game->players[i]->playerStats->numberOfBombes = 20;
        game->players[i]->playerStats->numberOfBombesPlanted = 2;
        game->players[i]->playerStats->speed = 300;
        game->players[i]->playerStats->lives = 1;
        game->players[i]->direction = 1;
        for(int j = 0; j < 4; j++)
        {
            game->players[i]->playerBombes->bombePosition[j].x = 0;
            game->players[i]->playerBombes->bombePosition[j].y = 0;
            game->players[i]->playerBombes->bombePosition[j].w = 48;
            game->players[i]->playerBombes->bombePosition[j].h = 48;
            game->players[i]->playerBombes->bombeSpritePosition[j].x = 0;
            game->players[i]->playerBombes->bombeSpritePosition[j].y = 0;
            game->players[i]->playerBombes->bombeSpritePosition[j].w = 84;
            game->players[i]->playerBombes->bombeSpritePosition[j].h = 84;
            game->players[i]->playerBombes->activated[j] = 0;
            game->players[i]->playerBombes->exploded[j] = 0;
            game->players[i]->playerBombes->bombeSquarePosition[j].x = 0;
            game->players[i]->playerBombes->bombeSquarePosition[j].y = 0;
        }
    }
    // init the players surfaces
    SDL_Surface *surfacesPlayers[4][6];
    initSurfaces(surfacesPlayers);

    // init the players texture
    for(int k = 0; k < 4; k++)
    {
        for(int l = 0; l < 4; l++)
        {
            game->players[k]->playerTexture[l] = SDL_CreateTextureFromSurface(game->renderer, surfacesPlayers[k][l]);
        }
    }



    return game;
}

gameStruct *socketsInit(gameStruct *game){
    for(int i = 0; i < 4; i++)
    {
        game->sockets->clientSockets[i] = -1;
    }
    puts("sockets initialised correctly on -1");
    return game;
}

void initSurfaces(SDL_Surface *surfacesPlayers[4][6]){
    surfacesPlayers[0][0] = IMG_Load("./images/characters/characters_dos_1.png");
    surfacesPlayers[0][1] = IMG_Load("./images/characters/characters_face_1.png");
    surfacesPlayers[0][2] = IMG_Load("./images/characters/characters_coteD_1.png");
    surfacesPlayers[0][3] = IMG_Load("./images/characters/characters_coteG_1.png");
    surfacesPlayers[0][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[0][5] = IMG_Load("./images/characters/characters.png");

    surfacesPlayers[1][0] = IMG_Load("./images/characters/characters_dos_2.png");
    surfacesPlayers[1][1] = IMG_Load("./images/characters/characters_face_2.png");
    surfacesPlayers[1][2] = IMG_Load("./images/characters/characters_coteD_2.png");
    surfacesPlayers[1][3] = IMG_Load("./images/characters/characters_coteG_2.png");
    surfacesPlayers[1][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[1][5] = IMG_Load("./images/characters/characters.png");

    surfacesPlayers[2][0] = IMG_Load("./images/characters/characters_dos_3.png");
    surfacesPlayers[2][1] = IMG_Load("./images/characters/characters_face_3.png");
    surfacesPlayers[2][2] = IMG_Load("./images/characters/characters_coteD_3.png");
    surfacesPlayers[2][3] = IMG_Load("./images/characters/characters_coteG_3.png");
    surfacesPlayers[2][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[2][5] = IMG_Load("./images/characters/characters.png");

    surfacesPlayers[3][0] = IMG_Load("./images/characters/characters_dos_4.png");
    surfacesPlayers[3][1] = IMG_Load("./images/characters/characters_face_4.png");
    surfacesPlayers[3][2] = IMG_Load("./images/characters/characters_coteD_4.png");
    surfacesPlayers[3][3] = IMG_Load("./images/characters/characters_coteG_4.png");
    surfacesPlayers[3][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[3][5] = IMG_Load("./images/characters/characters.png");
}

gameStruct *initBonuses(int sizeX, int sizeY, gameStruct *game) {
    game->bonuses = malloc(sizeof(gameBonusGeneral));
    game->bonuses->bonusesArray = malloc(sizeof(bonus)*16);
    for(int i = 0; i < 16; i++)
    {
        game->bonuses->bonusesArray[i] = malloc(sizeof(bonus));
        game->bonuses->bonusesArray[i]->bonusType = -1;
        game->bonuses->bonusesArray[i]->bonusPosition.x = 0;
        game->bonuses->bonusesArray[i]->bonusPosition.y = 0;
        game->bonuses->bonusesArray[i]->bonusPosition.w = game->screenSize.x / sizeX;
        game->bonuses->bonusesArray[i]->bonusPosition.h = game->screenSize.y / sizeY;
    }

    SDL_Surface *surfaceBonus = IMG_Load("./images/bonus/bonus_1.png"); SDL_Surface *surfaceBonus_2 = IMG_Load("./images/bonus/bonus_2.png");
    SDL_Surface *surfaceBonus_3 = IMG_Load("./images/bonus/bonus_3.png"); SDL_Surface *surfaceBonus_4 = IMG_Load("./images/bonus/bonus_4.png");
    SDL_Surface *surfaceBonus_5 = IMG_Load("./images/bonus/bonus_5.png");
    if (!surfaceBonus || !surfaceBonus_2 || !surfaceBonus_3 || !surfaceBonus_4 || !surfaceBonus_5) {
        printf("Error : SDL Surface creation (image : \"bonus_1 / bonus_2 / bonus_3 / bonus_4 / bonus_5.png\"): %s \n",IMG_GetError());
        if (surfaceBonus) {
            SDL_FreeSurface(surfaceBonus);
        }
        if (surfaceBonus_2) {
            SDL_FreeSurface(surfaceBonus_2);
        }
        if (surfaceBonus_3) {
            SDL_FreeSurface(surfaceBonus_3);
        }
        if (surfaceBonus_4) {
            SDL_FreeSurface(surfaceBonus_4);
        }
        if (surfaceBonus_5) {
            SDL_FreeSurface(surfaceBonus_5);
        }
        gameDestroy(game);
        return NULL;
    } else {
        game->bonuses->bonusTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus);
        game->bonuses->bonusTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_2);
        game->bonuses->bonusTexture[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_3);
        game->bonuses->bonusTexture[3] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_4);
        game->bonuses->bonusTexture[4] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_5);
        if (!game->bonuses->bonusTexture[0] || !game->bonuses->bonusTexture[1] || !game->bonuses->bonusTexture[2] || !game->bonuses->bonusTexture[3] || !game->bonuses->bonusTexture[4]) {
            printf("Error : SDL Texture creation player: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceBonus); SDL_FreeSurface(surfaceBonus_2); SDL_FreeSurface(surfaceBonus_3); SDL_FreeSurface(surfaceBonus_4); SDL_FreeSurface(surfaceBonus_5);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBonus); SDL_FreeSurface(surfaceBonus_2); SDL_FreeSurface(surfaceBonus_3); SDL_FreeSurface(surfaceBonus_4); SDL_FreeSurface(surfaceBonus_5);
        printf("Player texture initialised correctly\n");
    }
    return game;
}

gameStruct *buttonsInit(gameStruct *game){
    game->menu->firstButtonPosition.x = game->screenSize_all.w / 3;
    game->menu->firstButtonPosition.y = game->screenSize_all.h / 1.5;
    game->menu->firstButtonPosition.w = game->screenSize_all.w / 3;
    game->menu->firstButtonPosition.h = (game->screenSize_all.w / 3) / 5;
    game->menu->firstButtonSpritePosition.x = 0;
    game->menu->firstButtonSpritePosition.y = 0;
    game->menu->firstButtonSpritePosition.w = 600;
    game->menu->firstButtonSpritePosition.h = 125;
    SDL_Surface *surfaceButtonOne = IMG_Load("./images/join_game.png");
    if (!surfaceButtonOne) {
        printf("Error : SDL Surface creation (image : \"join_game.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->firstButtonTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceButtonOne);
        if (!game->menu->firstButtonTexture) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceButtonOne);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceButtonOne);
        printf("button one texture initialised correctly\n");
    }

    game->menu->secondButtonPosition.x = game->screenSize_all.w / 3;
    game->menu->secondButtonPosition.y = game->screenSize_all.h / 1.2;
    game->menu->secondButtonPosition.w = game->screenSize_all.w / 3;
    game->menu->secondButtonPosition.h = (game->screenSize_all.w / 3) / 5;
    game->menu->secondButtonSpritePosition.x = 0;
    game->menu->secondButtonSpritePosition.y = 0;
    game->menu->secondButtonSpritePosition.w = 600;
    game->menu->secondButtonSpritePosition.h = 125;
    SDL_Surface *surfaceButtonTwo = IMG_Load("./images/create_game.png");
    if (!surfaceButtonTwo) {
        printf("Error : SDL Surface creation (image : \"create_game.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->secondButtonTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceButtonTwo);
        if (!game->menu->secondButtonTexture) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceButtonTwo);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceButtonTwo);
        printf("button one texture initialised correctly\n");
    }

    game->menu->selectPortPosition.x = game->screenSize_all.w / 6;
    game->menu->selectPortPosition.y = game->screenSize_all.h / 1.3;
    game->menu->selectPortPosition.w = game->screenSize_all.w / 1.5;
    game->menu->selectPortPosition.h = (game->screenSize_all.w / 3) / 5;
    SDL_Surface *surfaceSelectPort = IMG_Load("./images/port_client.png");
    if (!surfaceSelectPort) {
        printf("Error : SDL Surface creation (image : \"port_client.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->selectPortTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceSelectPort);
        if (!game->menu->selectPortTexture) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceSelectPort);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceSelectPort);
        printf("button one texture initialised correctly\n");
    }

    game->menu->selectIpPosition.x = game->screenSize_all.w / 6;
    game->menu->selectIpPosition.y = game->screenSize_all.h / 1.3;
    game->menu->selectIpPosition.w = game->screenSize_all.w / 1.5;
    game->menu->selectIpPosition.h = (game->screenSize_all.w / 3) / 5;
    SDL_Surface *surfaceSelectIp = IMG_Load("./images/ip_client.png");
    if (!surfaceSelectIp) {
        printf("Error : SDL Surface creation (image : \"ip_client.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->selectIpTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceSelectIp);
        if (!game->menu->selectIpTexture) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceSelectIp);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceSelectIp);
        printf("button one texture initialised correctly\n");
    }

    game->menu->selectInputPosition.x = game->screenSize_all.w / 6;
    game->menu->selectInputPosition.y = game->screenSize_all.h / 1.3 + ((game->screenSize_all.w / 3) / 6);
    game->menu->selectInputPosition.w = game->screenSize_all.w / 4;
    game->menu->selectInputPosition.h = (game->screenSize_all.w / 3) / 5;
    SDL_Surface *surfaceSelectInput = IMG_Load("./images/input.png");
    if (!surfaceSelectInput) {
        printf("Error : SDL Surface creation (image : \"input.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->selectInputTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceSelectInput);
        if (!game->menu->selectInputTexture) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceSelectInput);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceSelectInput);
        printf("button one texture initialised correctly\n");
    }
    game->menu->inputPosition.x = game->screenSize_all.w / 5.8;
    game->menu->inputPosition.y = game->screenSize_all.h / 1.1;
    game->menu->inputPosition.w = game->screenSize.x / 16;
    game->menu->inputPosition.h = game->screenSize.y / 16;

    game->menu->errorPosition[0].x = 0;
    game->menu->errorPosition[0].y = game->screenSize_all.h / 16;
    game->menu->errorPosition[0].w = game->screenSize_all.w / 4;
    game->menu->errorPosition[0].h = (game->screenSize_all.w / 3) / 5;
    game->menu->errorSpritePosition[0].x = 0;
    game->menu->errorSpritePosition[0].y = 0;
    game->menu->errorSpritePosition[0].w = 600;
    game->menu->errorSpritePosition[0].h = 125;
    game->menu->errorPosition[1].x = 0;
    game->menu->errorPosition[1].y = game->screenSize_all.h / 16;
    game->menu->errorPosition[1].w = game->screenSize_all.w / 4;
    game->menu->errorPosition[1].h = (game->screenSize_all.w / 3) / 5;
    game->menu->errorSpritePosition[1].x = 0;
    game->menu->errorSpritePosition[1].y = 0;
    game->menu->errorSpritePosition[1].w = 600;
    game->menu->errorSpritePosition[1].h = 125;

    SDL_Surface *surfaceError_1 = IMG_Load("./images/wrong_port_or_ip.png"); SDL_Surface *surfaceError_2 = IMG_Load("./images/could_not_connect.png");
    if (!surfaceError_1 || !surfaceError_2) {
        printf("Error : SDL Surface creation (image : \"wrong_port_or_ip / could_not_connect.png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->errorTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceError_1);
        game->menu->errorTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceError_2);
        if (!game->menu->errorTexture[0] || !game->menu->errorTexture[1]) {
            printf("Error : SDL Texture creation errors textures: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceError_1); SDL_FreeSurface(surfaceError_2);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceError_1); SDL_FreeSurface(surfaceError_2);
        printf("errors textures initilised correctly\n");
    }

    game->menu->mapSelectionPosition[0].x = 0;
    game->menu->mapSelectionPosition[0].y = game->screenSize.y / 3;
    game->menu->mapSelectionPosition[0].w = game->screenSize_all.w / 3;;
    game->menu->mapSelectionPosition[0].h = game->menu->mapSelectionPosition[0].w / 1.36;

    game->menu->mapSelectionPosition[1].x = game->screenSize_all.w / 3;
    game->menu->mapSelectionPosition[1].y = game->screenSize.y / 3;
    game->menu->mapSelectionPosition[1].w = game->screenSize_all.w / 3;;
    game->menu->mapSelectionPosition[1].h = game->menu->mapSelectionPosition[1].w / 1.36;

    game->menu->mapSelectionPosition[2].x = (game->screenSize_all.w * 2) / 3;
    game->menu->mapSelectionPosition[2].y = game->screenSize.y / 3;
    game->menu->mapSelectionPosition[2].w = game->screenSize_all.w / 3;
    game->menu->mapSelectionPosition[2].h = game->menu->mapSelectionPosition[2].w / 1.36;

    game->menu->mapSelectionSpritePosition[0].x = 0;
    game->menu->mapSelectionSpritePosition[0].y = 0;
    game->menu->mapSelectionSpritePosition[0].w = 1000;;
    game->menu->mapSelectionSpritePosition[0].h = 733;

    game->menu->mapSelectionSpritePosition[1].x = 0;
    game->menu->mapSelectionSpritePosition[1].y = 0;
    game->menu->mapSelectionSpritePosition[1].w = 1000;;
    game->menu->mapSelectionSpritePosition[1].h = 733;

    game->menu->mapSelectionSpritePosition[2].x = 0;
    game->menu->mapSelectionSpritePosition[2].y = 0;
    game->menu->mapSelectionSpritePosition[2].w = 1000;
    game->menu->mapSelectionSpritePosition[2].h = 733;
    SDL_Surface *surfaceMapSelect_1 = IMG_Load("./images/map/map0.png");
    if (!surfaceMapSelect_1) {
        printf("Error : SDL Surface creation (image : \"map0\" .png\"): %s \n",IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->mapSelectionTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceMapSelect_1);

        if (!game->menu->mapSelectionTexture[0]) {
            printf("Error : SDL Texture creation input: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceMapSelect_1);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceMapSelect_1);
        printf("maps selection textures initialised correctly\n");
    }
    return game;
}

gameStruct *initHud(gameStruct *game) {
    for(int i = 0; i < 4; i++)
    {
        game->hud[i] = malloc(sizeof(hudStruct));
        if (i == 0) {
            game->hud[i]->hudPosition.x = game->screenSize.x;
            game->hud[i]->hudPosition.y = 0;
            game->hud[i]->hudPosition.h = game->screenSize_all.h / 4;
            game->hud[i]->hudPosition.w = game->screenSize_all.w - game->screenSize.x;

            // game->hud[i]->playerNumberPosition.x = game->hud[i]->hudPosition.x + game->hud[i]->hudPosition.w / 1.5;
            // game->hud[i]->playerNumberPosition.y = game->hud[i]->hudPosition.h / 8;
            // game->hud[i]->playerNumberPosition.h = 0;
            // game->hud[i]->playerNumberPosition.w = 0;

            game->hud[i]->alivePosition.x = game->hud[i]->hudPosition.x + game->hud[i]->hudPosition.w / 3;
            game->hud[i]->alivePosition.y = (game->hud[i]->hudPosition.h / 13.1);
            game->hud[i]->alivePosition.h = 0;
            game->hud[i]->alivePosition.w = 0;

            game->hud[i]->numberOfBombesPosition.x = game->hud[i]->hudPosition.x + game->hud[i]->hudPosition.w / 3;
            game->hud[i]->numberOfBombesPosition.y = (game->hud[i]->hudPosition.h / 24) * 7.05;
            game->hud[i]->numberOfBombesPosition.h = 0;
            game->hud[i]->numberOfBombesPosition.w = 0;

            game->hud[i]->numberOfBombesPlantedPosition.x = game->hud[i]->hudPosition.x + game->hud[i]->hudPosition.w / 3;
            game->hud[i]->numberOfBombesPlantedPosition.y = (game->hud[i]->hudPosition.h / 8) * 3.6;
            game->hud[i]->numberOfBombesPlantedPosition.h = 0;
            game->hud[i]->numberOfBombesPlantedPosition.w = 0;

            game->hud[i]->radiusOfBombesPosition.x = game->hud[i]->hudPosition.x + ((game->hud[i]->hudPosition.w / 6) * 5);
            game->hud[i]->radiusOfBombesPosition.y = (game->hud[i]->hudPosition.h / 13.1);
            game->hud[i]->radiusOfBombesPosition.h = 0;
            game->hud[i]->radiusOfBombesPosition.w = 0;

            game->hud[i]->speedPosition.x = game->hud[i]->hudPosition.x + game->hud[i]->hudPosition.w / 3;
            game->hud[i]->speedPosition.y = (game->hud[i]->hudPosition.h / 8) * 5.1;
            game->hud[i]->speedPosition.h = 0;
            game->hud[i]->speedPosition.w = 0;
        } else {
            game->hud[i]->hudPosition.x = game->hud[i-1]->hudPosition.x;
            game->hud[i]->hudPosition.y = game->hud[i-1]->hudPosition.y + game->hud[i-1]->hudPosition.h;
            game->hud[i]->hudPosition.h = game->hud[i-1]->hudPosition.h;
            game->hud[i]->hudPosition.w = game->hud[i-1]->hudPosition.w;

            // game->hud[i]->playerNumberPosition.x = game->hud[i-1]->playerNumberPosition.x;
            // game->hud[i]->playerNumberPosition.y = game->hud[i-1]->playerNumberPosition.y + game->hud[i-1]->hudPosition.h;
            // game->hud[i]->playerNumberPosition.h = game->hud[i-1]->playerNumberPosition.h;
            // game->hud[i]->playerNumberPosition.w = game->hud[i-1]->playerNumberPosition.w;

            game->hud[i]->alivePosition.x = game->hud[i-1]->alivePosition.x;
            game->hud[i]->alivePosition.y = game->hud[i-1]->alivePosition.y + game->hud[i-1]->hudPosition.h;
            game->hud[i]->alivePosition.h = game->hud[i-1]->alivePosition.h;
            game->hud[i]->alivePosition.w = game->hud[i-1]->alivePosition.w;

            game->hud[i]->numberOfBombesPosition.x = game->hud[i-1]->numberOfBombesPosition.x;
            game->hud[i]->numberOfBombesPosition.y = game->hud[i-1]->numberOfBombesPosition.y + game->hud[i-1]->hudPosition.h;
            game->hud[i]->numberOfBombesPosition.h = game->hud[i-1]->numberOfBombesPosition.h;
            game->hud[i]->numberOfBombesPosition.w = game->hud[i-1]->numberOfBombesPosition.w;

            game->hud[i]->radiusOfBombesPosition.x = game->hud[i-1]->radiusOfBombesPosition.x;
            game->hud[i]->radiusOfBombesPosition.y = game->hud[i-1]->radiusOfBombesPosition.y + game->hud[i-1]->hudPosition.h;
            game->hud[i]->radiusOfBombesPosition.h = game->hud[i-1]->radiusOfBombesPosition.h;
            game->hud[i]->radiusOfBombesPosition.w = game->hud[i-1]->radiusOfBombesPosition.w;

            game->hud[i]->numberOfBombesPlantedPosition.x = game->hud[i-1]->numberOfBombesPlantedPosition.x;
            game->hud[i]->numberOfBombesPlantedPosition.y = game->hud[i-1]->numberOfBombesPlantedPosition.y + game->hud[i-1]->hudPosition.h;
            game->hud[i]->numberOfBombesPlantedPosition.h = game->hud[i-1]->numberOfBombesPlantedPosition.h;
            game->hud[i]->numberOfBombesPlantedPosition.w = game->hud[i-1]->numberOfBombesPlantedPosition.w;

            game->hud[i]->speedPosition.x = game->hud[i-1]->speedPosition.x;
            game->hud[i]->speedPosition.y = game->hud[i-1]->speedPosition.y + game->hud[i-1]->hudPosition.h;
            game->hud[i]->speedPosition.h = game->hud[i-1]->speedPosition.h;
            game->hud[i]->speedPosition.w = game->hud[i-1]->speedPosition.w;
        }
        game->backgroundPosition_hud.x = game->screenSize.x;
        game->backgroundPosition_hud.y = 0;
        game->backgroundPosition_hud.h = game->screenSize.y;
        game->backgroundPosition_hud.w = game->screenSize_all.w - game->screenSize.x;
        // game->hud[i]->playerNumberTexture = changeText_(game, "player n : 0\0", 12);
        char *stat = malloc(sizeof(char)*4);
        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->playerStats->lives);
        game->hud[i]->aliveTexture = changeText_fixedSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->playerStats->numberOfBombes);
        game->hud[i]->numberOfBombesTexture = changeText_fixedSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->playerStats->numberOfBombesPlanted);
        game->hud[i]->numberOfBombesPlantedTexture = changeText_fixedSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->playerStats->radiusOfBombes);
        game->hud[i]->radiusOfBombesTexture = changeText_fixedSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->playerStats->speed);
        game->hud[i]->speedTexture = changeText_fixedSize_12(game, stat);
        // SDL_QueryTexture(game->hud[i]->playerNumberTexture, NULL, NULL, &game->hud[i]->playerNumberPosition.w, &game->hud[i]->playerNumberPosition.h);
        SDL_QueryTexture(game->hud[i]->aliveTexture, NULL, NULL, &game->hud[i]->alivePosition.w, &game->hud[i]->alivePosition.h);
        SDL_QueryTexture(game->hud[i]->numberOfBombesTexture, NULL, NULL, &game->hud[i]->numberOfBombesPosition.w, &game->hud[i]->numberOfBombesPosition.h);
        SDL_QueryTexture(game->hud[i]->numberOfBombesPlantedTexture, NULL, NULL, &game->hud[i]->numberOfBombesPlantedPosition.w, &game->hud[i]->numberOfBombesPlantedPosition.h);
        SDL_QueryTexture(game->hud[i]->radiusOfBombesTexture, NULL, NULL, &game->hud[i]->radiusOfBombesPosition.w, &game->hud[i]->radiusOfBombesPosition.h);
        SDL_QueryTexture(game->hud[i]->speedTexture, NULL, NULL, &game->hud[i]->speedPosition.w, &game->hud[i]->speedPosition.h);
    }
    SDL_Surface *surfaceHud = IMG_Load("./images/hud/hud_p1.png"); SDL_Surface *surfaceHud_2 = IMG_Load("./images/hud/hud_p2.png");
    SDL_Surface *surfaceHud_3 = IMG_Load("./images/hud/hud_p3.png"); SDL_Surface *surfaceHud_4 = IMG_Load("./images/hud/hud_p4.png");
    SDL_Surface *surfaceHud_dead = IMG_Load("./images/hud/hud_p1_dead.png"); SDL_Surface *surfaceHud_2_dead = IMG_Load("./images/hud/hud_p2_dead.png");
    SDL_Surface *surfaceHud_3_dead = IMG_Load("./images/hud/hud_p3_dead.png"); SDL_Surface *surfaceHud_4_dead = IMG_Load("./images/hud/hud_p4_dead.png");
    SDL_Surface *surfaceHud_background_1 = IMG_Load("./images/hud/background_hudMap0.png");
    SDL_Surface *surfaceHud_background_2 = IMG_Load("./images/hud/background_hudMap1.png");
    SDL_Surface *surfaceHud_background_3 = IMG_Load("./images/hud/background_hudMap2.png");
    if (!surfaceHud || !surfaceHud_2 || !surfaceHud_3 || !surfaceHud_4 || !surfaceHud_dead || !surfaceHud_2_dead || !surfaceHud_3_dead || !surfaceHud_4_dead || !surfaceHud_background_1 || !surfaceHud_background_2 || !surfaceHud_background_3) {
        printf("Error : SDL Surface creation (image : \"hud_p1 / hud_p2 / bonus_3 / hud_p3 / hud_p4.png\"): %s \n",IMG_GetError());
        if (surfaceHud) {
            SDL_FreeSurface(surfaceHud);
        }
        if (surfaceHud_2) {
            SDL_FreeSurface(surfaceHud_2);
        }
        if (surfaceHud_3) {
            SDL_FreeSurface(surfaceHud_3);
        }
        if (surfaceHud_4) {
            SDL_FreeSurface(surfaceHud_4);
        }
        if (surfaceHud_dead) {
            SDL_FreeSurface(surfaceHud_dead);
        }
        if (surfaceHud_2_dead) {
            SDL_FreeSurface(surfaceHud_2_dead);
        }
        if (surfaceHud_3_dead) {
            SDL_FreeSurface(surfaceHud_3_dead);
        }
        if (surfaceHud_4_dead) {
            SDL_FreeSurface(surfaceHud_4_dead);
        }
        if (surfaceHud_background_1) {
            SDL_FreeSurface(surfaceHud_background_1);
        }
        if (surfaceHud_background_2) {
            SDL_FreeSurface(surfaceHud_background_2);
        }
        if (surfaceHud_background_3) {
            SDL_FreeSurface(surfaceHud_background_3);
        }
        gameDestroy(game);
        return NULL;
    } else {
        game->hud[0]->hudTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud);
        game->hud[1]->hudTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_2);
        game->hud[2]->hudTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_3);
        game->hud[3]->hudTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_4);
        game->hud[0]->hudTextureDead = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_dead);
        game->hud[1]->hudTextureDead = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_2_dead);
        game->hud[2]->hudTextureDead = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_3_dead);
        game->hud[3]->hudTextureDead = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_4_dead);
        game->backgroundTexture_hud[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_background_1);
        game->backgroundTexture_hud[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_background_2);
        game->backgroundTexture_hud[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_background_3);
        if (!game->hud[0]->hudTexture || !game->hud[1]->hudTexture || !game->hud[2]->hudTexture || !game->hud[3]->hudTexture || !game->hud[0]->hudTextureDead || !game->hud[1]->hudTextureDead || !game->hud[2]->hudTextureDead || !game->hud[3]->hudTextureDead) {
            printf("Error : SDL Texture creation HUD: %s \n",IMG_GetError());
            SDL_FreeSurface(surfaceHud); SDL_FreeSurface(surfaceHud_2); SDL_FreeSurface(surfaceHud_3); SDL_FreeSurface(surfaceHud_4);
            SDL_FreeSurface(surfaceHud_dead); SDL_FreeSurface(surfaceHud_2_dead); SDL_FreeSurface(surfaceHud_3_dead); SDL_FreeSurface(surfaceHud_4_dead);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceHud); SDL_FreeSurface(surfaceHud_2); SDL_FreeSurface(surfaceHud_3); SDL_FreeSurface(surfaceHud_4);
        SDL_FreeSurface(surfaceHud_dead); SDL_FreeSurface(surfaceHud_2_dead); SDL_FreeSurface(surfaceHud_3_dead); SDL_FreeSurface(surfaceHud_4_dead);
        SDL_FreeSurface(surfaceHud_background_1); SDL_FreeSurface(surfaceHud_background_2); SDL_FreeSurface(surfaceHud_background_3);
        printf("HUD texture initialised correctly\n");
    }
    return game;
}
