#include"game2048.h"

void game2048::mainLoop_Playing(){
    SDL_Event eve;
    Uint32 time = SDL_GetTicks(), oldTime, deltaTime;
    int mousePosX, mousePosY;
    Uint32 mouseState;
    while(gamestate == game2048::PLAYING){
        oldTime = time;
        time = SDL_GetTicks();

        mouseState = SDL_GetMouseState(&mousePosX, &mousePosY);
        newGameButton.isCursorOnBtn(mousePosX, mousePosY);

        if(newGameButton.mouseOverState || tryagainButton.mouseOverState){
            SDL_SetCursor(cursorHand);
        }
        else{
            SDL_SetCursor(cursorArrow);
        }

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
                    //use for debug
                    // case SDLK_v: //reload configFile 
                    //     readConfig();
                    //     break;
                    // case SDLK_b:
                    //     gamestate = GAMEOVER;
                    //     break;
                }
            }
            else if(newGameButton.mouseOverState && eve.type == SDL_MOUSEBUTTONDOWN && eve.button.button == SDL_BUTTON_LEFT){
                initialGame();
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
    int mousePosX, mousePosY;
    Uint32 mouseState;
    while(gamestate == game2048::GAMEOVER){
        oldTime = time;
        time = SDL_GetTicks();

        mouseState = SDL_GetMouseState(&mousePosX, &mousePosY);
        newGameButton.isCursorOnBtn(mousePosX, mousePosY);
        tryagainButton.isCursorOnBtn(mousePosX, mousePosY);

        if(newGameButton.mouseOverState || tryagainButton.mouseOverState){
            SDL_SetCursor(cursorHand);
        }
        else{
            SDL_SetCursor(cursorArrow);
        }

        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                gamestate = game2048::EXIT;
            }
            else if((newGameButton.mouseOverState || tryagainButton.mouseOverState) 
                        && eve.type == SDL_MOUSEBUTTONDOWN && eve.button.button == SDL_BUTTON_LEFT){
                initialGame();
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

void game2048::Button::render(SDL_Renderer *renderer){
    SDL_RenderCopy(renderer, btnImg.texture, 
                    nullptr, &btnImg.rect);
}

bool game2048::Button::isCursorOnBtn(int x, int y){
    if(x < btnImg.rect.x || x > btnImg.rect.x + btnImg.rect.w ||
        y < btnImg.rect.y || y > btnImg.rect.y + btnImg.rect.h){
        mouseOverState = false;
        return false;
    }
    mouseOverState = true;
    return true;
}