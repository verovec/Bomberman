#include "../headers/main.h"

void changeText(gameStruct *game, char *message, int size, SDL_Rect rect, SDL_Surface *surface, SDL_Texture *texture){
    SDL_Color color = {30, 30, 30};
    game->menu->font = TTF_OpenFont("./fonts/font.ttf", size);
    surface = TTF_RenderText_Blended(game->menu->font, message, color);
    if (!surface) {
        printf("Error : SDL Surface modification menu: %s \n",SDL_GetError());
        gameDestroy(game);
    } else {
        texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        if (!texture) {
            printf("Error : SDL Texture modification menu: %s \n",SDL_GetError());
            gameDestroy(game);
        } else {
            SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
        }
    }
}

void gameDestroy(gameStruct *game) {
    if(game) {
        if(game->menu){
            if (game->menu->inputFont) {
                SDL_FreeSurface(game->menu->inputFont);
            }
            if (game->menu->inputTexture) {
                SDL_DestroyTexture(game->menu->inputTexture);
            }
            free(game->menu);
        } 
        if(game->bombe){
            if (game->bombe->bombeTexture) {
                SDL_DestroyTexture(game->bombe->bombeTexture);
            }
            if (game->bombe->flameTexture) {
                for(int i = 0; i < 5; i++)
                {
                    if (game->bombe->flameTexture[i]) {
                        SDL_DestroyTexture(game->bombe->flameTexture[i]);
                    }  
                }
            }
            free(game->bombe);
        } 
        if(game->players){
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 6; j++)
                {
                    if (game->players[i]->playerTexture[j]) {
                        SDL_DestroyTexture(game->players[i]->playerTexture[j]);
                    }
                    if (game->players[i]->playerBombes) {
                        free(game->players[i]->playerBombes);
                    }
                }
                free(game->players[i]);
            }  
        }
        if(game->infos){
            if (game->infos->port) {
                free(game->infos->port);
            }
            if (game->infos->ipAdresse) {
                free(game->infos->ipAdresse);
            }
            free(game->infos);
        }
        if(game->sockets){
            free(game->sockets);
        }
        if(game->map){
            for(int i = 0; i < game->numberOfBlocks_Y; i++)
            {
                for(int j = 0; j < game->numberOfBlocks_X; j++)
                {
                    // if (game->map[i][j]->squareTexture) {
                    //     free(game->map[i][j]->squareTexture);
                    // }
                    free(game->map[i][j]);
                }
            }
            
        }
        if (game->renderer) {
            SDL_DestroyRenderer(game->renderer);
        }
        if (game->window) {
            SDL_DestroyWindow(game->window);
        }
    SDL_Quit();
    free(game);
    }
}

SDL_Texture *changeText_fixedSize(gameStruct *game, char *message){
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color color = {30, 30, 30};
    surface = TTF_RenderText_Blended(game->menu->font, message, color);
    if (!surface) {
        printf("Error : SDL Surface creation menu: %s \n",SDL_GetError());
        gameDestroy(game);
    } else {
        texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        if (!texture) {
            printf("Error : SDL Texture creation menu: %s \n",SDL_GetError());
            gameDestroy(game);
        }
    }
    return texture;
}

SDL_Texture *changeText_fixedSize_12(gameStruct *game, char *message){
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color color = {30, 30, 30};
    surface = TTF_RenderText_Blended(game->menu->font_12, message, color);
    if (!surface) {
        printf("Error : SDL Surface creation menu: %s \n",SDL_GetError());
        gameDestroy(game);
    } else {
        texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        if (!texture) {
            printf("Error : SDL Texture creation menu: %s \n",SDL_GetError());
            gameDestroy(game);
        }
    }
    return texture;
}

