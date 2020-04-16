#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<mySDL2Tools/MySDLStruct.h>
#include"game2048.h"

int frame = 40;
int msPerFrame;
Uint32 moveAnimalTimeLength;
std::string textFontFilename;

int scr_width = 800, scr_height = 700;
SDL_Color backgroundColor = {.r = 196, .g = 158, .b = 117, .a = 255};
// SDL_Color planeColor = {.r = 205, .g = 193, .b = 180, .a = 255};
SDL_Color screenBackgroundColor = {.r = 255, .g = 255, .b = 255, .a = 255};
const char* configFilename = "config.lua";
SDL_Cursor *cursorArrow, *cursorHand;


TTF_Font *textFont;

void readConfigFile();

int main(int argv, char* argc[]){
    readConfigFile();
    msPerFrame = 1000 / frame;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("2048", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        scr_width, scr_height,
                                        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    textFont = TTF_OpenFont(textFontFilename.c_str(), 100);

    SDL_RenderSetLogicalSize(renderer, scr_width, scr_height);
    int posX = 0.2 * scr_width;
    int posY = 0.2 * scr_height;
    int width = 0.6 * scr_width;
    int spacing = 15;
    SDL_Rect renderRect = {.x = posX, .y = posY, .w = width, .h = width};

    game2048 GameMain{renderer, backgroundColor, renderRect,
                        (width - 3 * spacing) / 4, spacing};

    cursorArrow = SDL_GetCursor();
    cursorHand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    while(GameMain.gamestate != game2048::EXIT){
        switch(GameMain.gamestate){
            case game2048::INITIAL:
                GameMain.initialGame();
                break;
            case game2048::PLAYING:
                GameMain.mainLoop_Playing();
                break;
            case game2048::GAMEOVER:
                GameMain.mainLoop_GameOver();
                break;
            case game2048::WIN:
                GameMain.mainLoop_Win();
        }

    }
    
    TTF_CloseFont(textFont);
    TTF_Quit();
    SDL_FreeCursor(cursorHand);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}


void configReadString(lua_State *L, const char* varName, std::string & var){
    lua_getglobal(L, varName);
    var = lua_tostring(L, -1);
    lua_pop(L, 1);
}

int configReadInteger(lua_State *L, const char* varName){
    int var;
    lua_getglobal(L, varName);
    int isnum;
    var = lua_tonumberx(L, -1, &isnum);
    lua_pop(L, 1);
    if(!isnum){
        std::cout << "Failed to reand integer " << varName << std::endl;
        throw std::runtime_error("read integer failed");
    }
    return var;
}

//table is at top of stack before call of this function.
int configReadTableIntComponent(lua_State *L, const char *componentName){
    int var;
    lua_pushstring(L, componentName);
    lua_gettable(L, -2);
    var = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return var;
}

void readConfigFile(){
    lua_State *L = luaL_newstate();
    if(luaL_loadfile(L, configFilename)|| lua_pcall(L, 0, 0, 0)){
        std::cout << "Failed to load config File" << std::endl;
        std::cout << lua_tostring(L, -1);
    }

    frame = configReadInteger(L, "frame");
    scr_width = configReadInteger(L, "scr_width");
    scr_height = configReadInteger(L, "scr_height");
    moveAnimalTimeLength = configReadInteger(L, "moveAnimationTimeLength");
    configReadString(L, "textFontFilename", textFontFilename);

    lua_close(L);
}