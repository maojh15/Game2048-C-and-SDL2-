#ifndef GAME_2048_H_
#define GAME_2048_H_
#include<iostream>
#include<mySDL2Tools/MySDLStruct.h>
#include<random>
#include<ctime>
#include<vector>
#include<cmath>
#include<lua.hpp>
#include<string>
#define PI 3.1415926

extern SDL_Color screenBackgroundColor;
extern SDL_Color backgroundColor;
extern int msPerFrame;
extern Uint32 moveAnimalTimeLength;
extern const char *configFilename;

extern void configReadString(lua_State *L, const char* varName, std::string & var);
extern int configReadInteger(lua_State *L, const char* varName);
extern int configReadTableIntComponent(lua_State *L, const char *componentName);
extern SDL_Cursor *cursorArrow, *cursorHand;

class game2048{
public:
    enum GameState{
        INITIAL,
        NEWGAME,
        PLAYING,
        GAMEOVER,
        EXIT,
        TOP
    };

    class Button{
    public:
        Button ():mouseOverState{false}{}
        void render(SDL_Renderer *renderer);
        bool isCursorOnBtn(int x, int y); // test whether mouse cursor is over the button.
        
        TextureWrap btnImg;
        bool mouseOverState;

    };

    game2048(SDL_Renderer *renderer_, SDL_Color backgroundColor,
                SDL_Rect renderRect_, int sizeOfBlock_, int spacing_):
                bgColor{backgroundColor}, renderer{renderer_},
                renderRect{renderRect_}, blockSize{sizeOfBlock_},
                spacing{spacing_}, gamestate{INITIAL}{
        if(!readConfig()){
            std::cout << "Failed to read config!" << std::endl;
            throw -1;
        }
        loadImgSrc();
        blockInImgRect.w = 100;
        blockInImgRect.h = 100;
        randomEngine.seed(std::time(0));
    }
    
    void loadImgSrc(){
        texture.createFromIMG(renderer, imgSrcFilename.c_str());
        newGameButton.btnImg.createFromIMG(renderer, newGameButtonImageFilename.c_str());
        gameoverImg.createFromIMG(renderer, gameoverImgFilename.c_str());
        gameoverImg.rect.x -= gameoverImg.rect.w / 2;
        gameoverImg.rect.y -= gameoverImg.rect.h / 2;

        tryagainButton.btnImg.createFromIMG(renderer, tryagainButtonImgFilename.c_str());
        tryagainButton.btnImg.rect.x -= tryagainButton.btnImg.rect.w / 2;
        tryagainButton.btnImg.rect.y -= tryagainButton.btnImg.rect.h / 2;
    }
    
    void getPosition(int i, int j){
        tempRect = {.x = renderRect.x + j * (spacing + blockSize),
                    .y = renderRect.y + i * (spacing + blockSize),
                    .w = blockSize,
                    .h = blockSize};
    }

    void renderBlock(int index, int i, int j){
        int x, y;
        x = index % maxWidthHeight;
        y = index / maxWidthHeight;
        blockInImgRect.x = x * 110;
        blockInImgRect.y = y * 110;
        getPosition(i, j);
        SDL_RenderCopy(renderer, texture.texture, &blockInImgRect, &tempRect);
    }

    void renderBlockAtPos(int index, int posX, int posY){
        int x, y;
        x = index % maxWidthHeight;
        y = index / maxWidthHeight;
        blockInImgRect.x = x * 110;
        blockInImgRect.y = y * 110;
        tempRect = {.x = posX, .y = posY, .w = blockSize, .h = blockSize};
        SDL_RenderCopy(renderer, texture.texture, &blockInImgRect, &tempRect);
    }

    void render(){
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g,
                                bgColor.b, bgColor.a);
        newGameButton.render(renderer);
        
        SDL_RenderFillRect(renderer, &renderRect);
        for(int i = 0; i < maxWidthHeight; ++i){
            for(int j = 0; j < maxWidthHeight; ++j){
                renderBlock(data[i][j], i, j);
            }
        }

        if(gamestate == GAMEOVER){
            SDL_RenderCopy(renderer, gameoverImg.texture, 
                            nullptr, &gameoverImg.rect);
            SDL_RenderCopy(renderer, tryagainButton.btnImg.texture,
                            nullptr, &tryagainButton.btnImg.rect);
        }

    }

    void initialGame(){
        for(int i = 0; i < maxWidthHeight; ++i)
            for(int j = 0; j < maxWidthHeight; ++j)
                data[i][j] = 0;
        int x1, y1, x2, y2;
        x1 = randomPos(randomEngine);
        y1 = randomPos(randomEngine);  
        data[x1][y1] = randomNum(randomEngine);
        do{
            x2 = randomPos(randomEngine);
            y2 = randomPos(randomEngine);
        }while(x2 == x1 && y2 == y1);
        data[x2][y2] = randomNum(randomEngine);
        gamestate = PLAYING;
    }

    //direction = 0, 1, 2, 3 correspond to four directions up, right, down, left respectively;
    void moveBlocks(int direction){
        int temp1;
        switch(direction){
            case 0: //up
                for(int j = 0; j < maxWidthHeight; ++j){
                    for(int i = 1; i < maxWidthHeight; ++i){
                        if(data[i][j] != 0){
                            temp1 = i - 1;
                            while(temp1 >= 0 && data[temp1][j] == 0){
                                //find nearest non-empty object.
                                --temp1;
                            }
                            if(temp1 < 0){ // empty at position temp1==0, j
                                animationList.push_back(motionPath(i, j, 0, j, data[i][j], 0));
                                data[0][j] = data[i][j];
                                data[i][j] = 0;
                            }
                            else if(data[temp1][j] == data[i][j]){ // merge
                                animationList.push_back(motionPath(i, j, temp1, j, data[i][j], data[i][j]));
                                ++data[temp1][j];
                                data[i][j] = 0;
                            }
                            else if(temp1 < i - 1){
                                animationList.push_back(motionPath(i, j, temp1 + 1, j, data[i][j], 0));
                                data[temp1 + 1][j] = data[i][j];
                                data[i][j] = 0;
                            }
                        }
                    }
                }
                break;
            case 1: //right
                for(int i = 0; i < maxWidthHeight; ++i){
                    for(int j = maxWidthHeight - 2; j >= 0; --j){
                        if(data[i][j] != 0){
                            temp1 = j + 1;
                            while(temp1 < maxWidthHeight && data[i][temp1] == 0){
                                //find nearest non-empty object.
                                ++temp1;
                            }
                            if(temp1 == maxWidthHeight){
                                animationList.push_back(motionPath(i, j, i, maxWidthHeight - 1, data[i][j], 0));
                                data[i][maxWidthHeight - 1] = data[i][j];
                                data[i][j] = 0;
                            }
                            else if(data[i][temp1] == data[i][j]){ // merge
                                animationList.push_back(motionPath(i, j, i, temp1, data[i][j], data[i][temp1]));
                                ++data[i][temp1];
                                data[i][j] = 0;
                            }
                            else if(temp1 > j + 1){
                                animationList.push_back(motionPath(i, j, i, temp1 - 1, data[i][j], 0));
                                data[i][temp1 - 1] = data[i][j];
                                data[i][j] = 0;
                            }
                        }
                    }
                }
                break;
            case 2: //down
                for(int j = 0; j < maxWidthHeight; ++j){
                    for(int i = maxWidthHeight - 2; i >= 0; --i){
                        if(data[i][j] != 0){
                            temp1 = i + 1;
                            while(temp1 < maxWidthHeight && data[temp1][j] == 0){
                                //find nearest non-empty object.
                                ++temp1;
                            }
                            if(temp1 == maxWidthHeight){
                                animationList.push_back(motionPath(i, j, maxWidthHeight - 1, j, data[i][j], 0));
                                data[maxWidthHeight - 1][j] = data[i][j];
                                data[i][j] = 0;
                            }
                            else if(data[temp1][j] == data[i][j]){ // merge
                                animationList.push_back(motionPath(i, j, temp1, j, data[i][j], data[i][j]));
                                ++data[temp1][j];
                                data[i][j] = 0;
                            }
                            else if(temp1 > i + 1){
                                animationList.push_back(motionPath(i, j, temp1 - 1, j, data[i][j], 0));
                                data[temp1 - 1][j] = data[i][j];
                                data[i][j] = 0;
                            }
                        }
                    }
                }
                break;
            case 3: //left
                for(int i = 0; i < maxWidthHeight; ++i){
                    for(int j = 1; j < maxWidthHeight; ++j){
                        if(data[i][j] != 0){
                            temp1 = j - 1;
                            while(temp1 >= 0 && data[i][temp1] == 0){
                                //find nearest non-empty object.
                                --temp1;
                            }
                            if(temp1 < 0){ // empty at position temp1==0, j
                                animationList.push_back(motionPath(i, j, i, 0, data[i][j], 0));
                                data[i][0] = data[i][j];
                                data[i][j] = 0;
                            }
                            else if(data[i][temp1] == data[i][j]){ // merge
                                animationList.push_back(motionPath(i, j, i, temp1, data[i][j], data[i][j]));;
                                ++data[i][temp1];
                                data[i][j] = 0;
                            }
                            else if(temp1 < j - 1){
                                animationList.push_back(motionPath(i, j, i, temp1 + 1, data[i][j], 0));
                                data[i][temp1 + 1] = data[i][j];
                                data[i][j] = 0;
                            }
                        }
                    }
                }
                break;
        }

        if(animationList.size() != 0){
            newBlock();
        }
    }

    void newBlock(){
        moveAnimation();

        //test empty block;
        for(int i = 0; i < maxWidthHeight; ++i){
            for(int j = 0; j < maxWidthHeight; ++j){
                if(data[i][j] == 0)
                    tempNum.push_back({i, j});
            }
        }
        int len = tempNum.size();
        std::uniform_int_distribution<int> u{0, len - 1};
        std::pair<int, int> tmpPair = tempNum[u(randomEngine)];
        data[tmpPair.first][tmpPair.second] = randomNum(randomEngine);

        if(len == 1){
            bool allAdjacentBlocksAreDifferent = true;
            //len == 1 means no empty block exist now.
            //We should check whether the game can continue.
            //If each pair of adjacent blocks are labeled by different number,
            //the game is over.
            for(int i = 0; i < maxWidthHeight - 1; ++i){
                for(int j = 0; j < maxWidthHeight - 1; ++j){
                    if(data[i][j] == data[i+1][j] || data[i][j] == data[i][j+1]){
                        allAdjacentBlocksAreDifferent = false;
                        break;
                    }
                }
                if(!allAdjacentBlocksAreDifferent)
                    break;
            }

            //boundary case
            if(allAdjacentBlocksAreDifferent){
                int i = maxWidthHeight - 1;
                for(int j = 0; j < maxWidthHeight - 1; ++j){
                    if(data[i][j] == data[i][j+1]){
                        allAdjacentBlocksAreDifferent = false;
                        break;
                    }

                }
            }
            if(allAdjacentBlocksAreDifferent){
                int j = maxWidthHeight - 1;
                for(int i = 0; i < maxWidthHeight - 1; ++i){
                    if(data[i][j] == data[i+1][j]){
                        allAdjacentBlocksAreDifferent = false;
                        break;
                    }
                }
            }


            if(allAdjacentBlocksAreDifferent)
                gamestate = GAMEOVER;
        }

        tempNum.clear();
    }

    void moveAnimation(){
        int len = animationList.size();
        //set initial position
        for(int i = 0; i < len; ++i){
            animationList[i].initPosX = renderRect.x + animationList[i].fromJ * (spacing + blockSize);
            animationList[i].initPosY = renderRect.y + animationList[i].fromI * (spacing + blockSize);
        }
        Uint32 time = SDL_GetTicks(), oldTime, deltaTime, timeCount = 0;
        std::pair<double, double> tmpVel;
        //play move animation
        while(timeCount <= moveAnimalTimeLength){
            oldTime = time;
            time = SDL_GetTicks();
            deltaTime = time - oldTime;
            timeCount += deltaTime;

            render();
            for(int i = 0; i < len; ++i){
                animationList[i].updatePos(timeCount, blockSize + spacing);
                renderBlock(animationList[i].targetNum, animationList[i].toI, animationList[i].toJ);
            }

            for(int i = 0; i < len; ++i){
                renderBlockAtPos(animationList[i].srcNum, animationList[i].posX, animationList[i].posY);
            }

            SDL_RenderPresent(renderer);

            deltaTime = SDL_GetTicks() - time;
            if(deltaTime < msPerFrame){
                SDL_Delay(msPerFrame - deltaTime);
            }

        }

        animationList.clear();
    }


    bool readConfig(){
        lua_State *L = luaL_newstate();
        if(luaL_loadfile(L, configFilename)||lua_pcall(L, 0, 0, 0)){
            std::cout << "Failed to load config file " << configFilename << std::endl;
            std::cout << lua_tostring(L, -1) << std::endl;
            return false;
        }
        configReadString(L, "imgSrcFilename", imgSrcFilename);
        configReadString(L, "newGameButtonImageFilename", newGameButtonImageFilename);
        configReadString(L, "gameoverImgFilename", gameoverImgFilename);
        configReadString(L, "tryagainButtonImgFilename", tryagainButtonImgFilename);

        lua_getglobal(L, "newGameButtonPos");
        newGameButton.btnImg.rect.x = configReadTableIntComponent(L, "x");
        newGameButton.btnImg.rect.y = configReadTableIntComponent(L, "y");
        lua_pop(L, 1);

        lua_getglobal(L, "gameoverImgCenterPos");
        gameoverImg.rect.x = configReadTableIntComponent(L, "x");
        gameoverImg.rect.y = configReadTableIntComponent(L, "y");
        lua_pop(L, 1);

        lua_getglobal(L, "tryagainBtnCenterPos");
        tryagainButton.btnImg.rect.x = configReadTableIntComponent(L, "x");
        tryagainButton.btnImg.rect.y = configReadTableIntComponent(L, "y");

        lua_close(L);
        return true;
    }

    //used for debug, test the render function
    void test(){
        std::uniform_int_distribution<int> u{0, 11};
        for(int i = 0; i < maxWidthHeight; ++i)
            for(int j = 0; j < maxWidthHeight; ++j)
                data[i][j] = u(randomEngine);

    }

    void mainLoop_Playing();
    void mainLoop_GameOver();


    static const int maxWidthHeight = 4; //4x4 lattice.

    int data[maxWidthHeight][maxWidthHeight];
    SDL_Color bgColor;
    SDL_Renderer *renderer;
    TextureWrap texture;
    TextureWrap gameoverImg;
    Button newGameButton, tryagainButton;
    int blockSize, spacing;
    SDL_Rect renderRect;
    SDL_Rect blockInImgRect;

    GameState gamestate;
    

private:
    struct motionPath{
        motionPath(int fi, int fj, int toi, int toj, int src_, int tag_)noexcept:
                    fromI{fi}, fromJ{fj}, toI{toi}, toJ{toj},
                    srcNum{src_}, targetNum{tag_}{
            periodicSin = PI / moveAnimalTimeLength;
        }
        std::pair<double, double> getVelocity(int timeCount){
            double tmp = std::sin(timeCount * periodicSin) * PI / moveAnimalTimeLength;
            return  {tmp * (toJ - fromJ) / 2.0, tmp * (toI - fromI) / 2.0};
        }

        void updatePos(int timeCount, int unitDistance){
            double tmp = (1 - std::cos(PI * timeCount / moveAnimalTimeLength)) * unitDistance / 2.0;
            deltaPosX = tmp * (toJ - fromJ);
            deltaPosY = tmp * (toI - fromI);
            posX = initPosX + deltaPosX;
            posY = initPosY + deltaPosY;
        }

        // std::pair<double, double> getVelocity(int timeCount){
        //     double tmp = 2.0  * timeCount / (moveAnimalTimeLength * moveAnimalTimeLength);
        //     return {tmp * (toJ - fromJ), tmp * (toI - fromI)};
        // }

        int fromI, fromJ;
        int toI, toJ;
        int targetNum; //the number at toi, toj (i.e. data[toi][toj])
        int srcNum;
        double posX, posY;
        double deltaPosX, deltaPosY;
        double initPosX, initPosY;
        double velocityCoeff;
        
    private:
        double periodicSin;
    };

    SDL_Rect tempRect;
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<int> randomPos{0, 3};
    std::uniform_int_distribution<int> randomNum{1, 2};
    std::vector<std::pair<int, int> > tempNum;
    std::vector<motionPath> animationList;

    std::string imgSrcFilename;
    std::string newGameButtonImageFilename;
    std::string gameoverImgFilename;
    std::string tryagainButtonImgFilename;

};

#endif