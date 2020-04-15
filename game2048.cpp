#include"game2048.h"
void game2048::mainLoop_Playing(){
    SDL_Event eve;
    Uint32 time = SDL_GetTicks(), oldTime, deltaTime;
    while(gamestate == game2048::PLAYING){
        oldTime = time;
        time = SDL_GetTicks();
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                gamestate = game2048::EXIT;
            }
            else if(eve.type == SDL_KEYDOWN){
                switch(eve.key.keysym.sym){
                    case SDLK_UP: //move up
                    case SDLK_w: 
                        moveBlocks(0);
                        break;
                    case SDLK_RIGHT: //move right
                    case SDLK_d:
                        moveBlocks(1);
                        break;
                    case SDLK_DOWN: //move down
                    case SDLK_s:
                        moveBlocks(2);
                        break;
                    case SDLK_LEFT: //move left
                    case SDLK_a:
                        moveBlocks(3);
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                backgroundColor.g, backgroundColor.b,
                                backgroundColor.a);
        SDL_RenderClear(renderer);

        render();

        SDL_RenderPresent(renderer);

        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame)
            SDL_Delay(msPerFrame - deltaTime);
    }
}

void game2048::mainLoop_GameOver(){
    SDL_Event eve;
    Uint32 time = SDL_GetTicks(), oldTime, deltaTime;
    while(gamestate == game2048::GAMEOVER){
        oldTime = time;
        time = SDL_GetTicks();
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                gamestate = game2048::EXIT;
            }
            
        }

        SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                backgroundColor.g, backgroundColor.b,
                                backgroundColor.a);
        SDL_RenderClear(renderer);

        render();

        SDL_RenderPresent(renderer);

        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame)
            SDL_Delay(msPerFrame - deltaTime);
    }
}