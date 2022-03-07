#include "../headers/main.h"

#define direction game->players[k]->direction
#define bombeX game->players[k]->playerBombes->bombeSquarePosition[j].x
#define bombeY game->players[k]->playerBombes->bombeSquarePosition[j].y
#define radiusBombe game->players[k]->playerStats->radiusOfBombes

void menuDraw(gameStruct *game, Uint32 *seconds) {
    // SDL_SetRenderDrawColor(game->renderer, 155,175,35,255);
    Uint32 otherSeconds = *seconds;
    SDL_RenderClear(game->renderer);

    if (game->menu->menuSelected <= 4) {
        SDL_RenderCopy(game->renderer, game->menu->backgroundTexture, NULL, &game->screenSize_all);
    } else {
        SDL_RenderCopy(game->renderer, game->menu->backgroundTexture_selectMap, NULL, &game->screenSize_all);
    }

    switch(game->menu->menuSelected) {
        case 1:
            game->menu->firstButtonSpritePosition = animationMenu(game->menu->firstButtonSpritePosition, seconds, 875, 125);
            game->menu->secondButtonSpritePosition.y = 0;
            SDL_RenderCopy(game->renderer, game->menu->firstButtonTexture, &game->menu->firstButtonSpritePosition, &game->menu->firstButtonPosition);
            SDL_RenderCopy(game->renderer, game->menu->secondButtonTexture, &game->menu->secondButtonSpritePosition, &game->menu->secondButtonPosition);
            break;
        case 2:
            game->menu->secondButtonSpritePosition = animationMenu(game->menu->secondButtonSpritePosition, seconds, 875, 125);
            game->menu->firstButtonSpritePosition.y = 0;
            SDL_RenderCopy(game->renderer, game->menu->firstButtonTexture, &game->menu->firstButtonSpritePosition, &game->menu->firstButtonPosition);
            SDL_RenderCopy(game->renderer, game->menu->secondButtonTexture, &game->menu->secondButtonSpritePosition, &game->menu->secondButtonPosition);
            break;
        case 3:
            SDL_RenderCopy(game->renderer, game->menu->selectPortTexture, NULL, &game->menu->selectPortPosition);
            SDL_RenderCopy(game->renderer, game->menu->selectInputTexture, NULL, &game->menu->selectInputPosition);
            break;
        case 4:
            SDL_RenderCopy(game->renderer, game->menu->selectIpTexture, NULL, &game->menu->selectIpPosition);
            SDL_RenderCopy(game->renderer, game->menu->selectInputTexture, NULL, &game->menu->selectInputPosition);
            break;
        default:
            break;
    }

        SDL_RenderCopy(game->renderer, game->menu->inputTexture, NULL, &game->menu->inputPosition);
        game->menu->errorSpritePosition[0].y = 0;
        game->menu->errorSpritePosition[1].y = 0; // just to reset the errors, may consume resources

    SDL_RenderPresent(game->renderer);
}

void gameDraw(gameStruct *game, int sizeX, int sizeY, int playersThatAreHere[4]) {
    SDL_SetRenderDrawColor(game->renderer, 255,255,255,255);
    SDL_RenderClear(game->renderer);
    SDL_RenderCopy(game->renderer, game->backgroundTexture_hud[game->mapCaracteristics->mapNumber], NULL, &game->backgroundPosition_hud);
    for(int i = 0; i < sizeY; i++) { // map
        for(int j = 0; j < sizeX; j++) {
            SDL_RenderCopy(game->renderer, game->mapCaracteristics->mapTextures[game->map[i][j]->squareType], NULL, &game->map[i][j]->squarePosition);
        }
    }
    for(int l = 0; l < 16; l++) { // bonuses
        if (game->bonuses->bonusesArray[l]->bonusType != -1) {
            SDL_RenderCopy(game->renderer, game->bonuses->bonusTexture[game->bonuses->bonusesArray[l]->bonusType], &game->bonuses->bonusesArray[l]->bonusSpritePosition, &game->bonuses->bonusesArray[l]->bonusPosition);
        }
    }
    int goodXplus; int goodXminus; int goodYplus; int goodYminus;
    goodXplus = 1; goodXminus = 1; goodYplus = 1; goodYminus = 1;
    for(int k = 0; k < 4; k++) { // bombes and players
        if (playersThatAreHere[k] == 1) {
            for(int j = 0; j < game->players[k]->playerStats->numberOfBombesPlanted; j++)
            {
                goodXplus = 1; goodXminus = 1; goodYplus = 1; goodYminus = 1;
                if (game->players[k]->playerBombes->activated[j]) {
                    SDL_RenderCopy(game->renderer, game->bombe->bombeTexture, &game->players[k]->playerBombes->bombeSpritePosition[j], &game->players[k]->playerBombes->bombePosition[j]);
                }
                if (game->players[k]->playerBombes->exploded[j]) {
                    SDL_RenderCopy(game->renderer, game->bombe->flameTexture[0], NULL, &game->players[k]->playerBombes->bombePosition[j]);
                    for(int l = 1; l < radiusBombe; l++){
                        if (goodYplus) {
                            if (game->map[bombeY+l][bombeX]->squareType < 3) {
                                SDL_RenderCopy(game->renderer, game->bombe->flameTexture[6], NULL, &game->map[bombeY+l][bombeX]->squarePosition);
                            } else {
                                goodYplus = 0;
                            }
                        }
                        if (goodYminus) {
                            if (game->map[bombeY-l][bombeX]->squareType < 3) {
                                SDL_RenderCopy(game->renderer, game->bombe->flameTexture[5], NULL, &game->map[bombeY-l][bombeX]->squarePosition);
                            } else {
                                goodYminus = 0;
                            }
                        }
                        if (goodXplus) {
                            if (game->map[bombeY][bombeX+l]->squareType < 3) {
                                SDL_RenderCopy(game->renderer, game->bombe->flameTexture[7], NULL, &game->map[bombeY][bombeX+l]->squarePosition);
                            } else {
                                goodXplus = 0;
                            }
                        }
                        if (goodXminus) {
                            if (game->map[bombeY][bombeX-l]->squareType < 3) {
                                SDL_RenderCopy(game->renderer, game->bombe->flameTexture[8], NULL, &game->map[bombeY][bombeX-l]->squarePosition);
                            } else {
                                goodXminus = 0;
                            }
                        }
                    }
                    if (goodYplus) {
                        if (game->map[bombeY+radiusBombe][bombeX]->squareType < 3) {
                                SDL_RenderCopy(game->renderer, game->bombe->flameTexture[2], NULL, &game->map[bombeY+radiusBombe][bombeX]->squarePosition);
                        }
                    }
                    if (goodYminus) {
                        if (game->map[bombeY-radiusBombe][bombeX]->squareType < 3) {
                            SDL_RenderCopy(game->renderer, game->bombe->flameTexture[1], NULL, &game->map[bombeY-radiusBombe][bombeX]->squarePosition);
                        }
                    }
                    if (goodXplus) {
                        if (game->map[bombeY][bombeX+radiusBombe]->squareType < 3) {
                            SDL_RenderCopy(game->renderer, game->bombe->flameTexture[3], NULL, &game->map[bombeY][bombeX+radiusBombe]->squarePosition);
                        }
                    }
                    if (goodXminus) {
                        if (game->map[bombeY][bombeX-radiusBombe]->squareType < 3) {
                            SDL_RenderCopy(game->renderer, game->bombe->flameTexture[4], NULL, &game->map[bombeY][bombeX-radiusBombe]->squarePosition);
                        }
                    }
                }
            }
            if (game->players[k]->playerStats->lives) {
                SDL_RenderCopy(game->renderer, game->players[k]->playerTexture[direction], &game->players[k]->playerSpritePosition, &game->players[k]->playerPosition);
                SDL_RenderCopy(game->renderer, game->hud[k]->hudTexture, NULL, &game->hud[k]->hudPosition);
                SDL_RenderCopy(game->renderer, game->hud[k]->aliveTexture, NULL, &game->hud[k]->alivePosition);
                SDL_RenderCopy(game->renderer, game->hud[k]->numberOfBombesTexture, NULL, &game->hud[k]->numberOfBombesPosition);
                SDL_RenderCopy(game->renderer, game->hud[k]->numberOfBombesPlantedTexture, NULL, &game->hud[k]->numberOfBombesPlantedPosition);
                SDL_RenderCopy(game->renderer, game->hud[k]->radiusOfBombesTexture, NULL, &game->hud[k]->radiusOfBombesPosition);
                SDL_RenderCopy(game->renderer, game->hud[k]->speedTexture, NULL, &game->hud[k]->speedPosition);
            } else {
                SDL_RenderCopy(game->renderer, game->hud[k]->hudTextureDead, NULL, &game->hud[k]->hudPosition);
            }
        } else {
            SDL_RenderCopy(game->renderer, game->hud[k]->hudTextureDead, NULL, &game->hud[k]->hudPosition);
        }
    }
    SDL_RenderPresent(game->renderer);
}

void menuDraw_serverRunning(gameStruct *game) {
    SDL_RenderClear(game->renderer);
    SDL_RenderCopy(game->renderer, game->menu->backgroundTexture_serverRunning, NULL, &game->screenSize_all);
    SDL_RenderPresent(game->renderer);
}
