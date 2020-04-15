#ifndef GAME_2048_H_
#define GAME_2048_H_
#include<iostream>
#include<mySDL2Tools/MySDLStruct.h>
#include<random>
#include<ctime>
#include<vector>
#include<cmath>
#define PI 3.1415926

extern SDL_Color screenBackgroundColor;
extern SDL_Color backgroundColor;
extern int msPerFrame;

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

    game2048(SDL_Renderer *renderer_, const char* imgFilename, SDL_Color backgroundColor,
                SDL_Rect renderRect_, int sizeOfBlock_, int spacing_):
                bgColor{backgroundColor}, renderer{renderer_},
                renderRect{renderRect_}, blockSize{sizeOfBlock_},
                spacing{spacing_}, gamestate{INITIAL}{
        loadImgSrc(imgFilename);
        blockInImgRect.w = 100;
        blockInImgRect.h = 100;
        randomEngine.seed(std::time(0));
    }
    
    void loadImgSrc(const char* imgFilename){
        texture.createFromIMG(renderer, imgFilename);
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
        SDL_RenderFillRect(renderer, &renderRect);
        for(int i = 0; i < maxWidthHeight; ++i){
            for(int j = 0; j < maxWidthHeight; ++j){
                renderBlock(data[i][j], i, j);
            }
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
        for(int i = 0; i < len; ++i){
            animationList[i].posX = renderRect.x + animationList[i].fromJ * (spacing + blockSize);
            animationList[i].posY = renderRect.y + animationList[i].fromI * (spacing + blockSize);
        }
        Uint32 time = SDL_GetTicks(), oldTime, deltaTime, timeCount = 0;
        std::pair<double, double> tmpVel;
        //play move animation
        while(timeCount <= motionPath::animalTimeLength){
            oldTime = time;
            time = SDL_GetTicks();
            deltaTime = time - oldTime;
            timeCount += deltaTime;

            render();
            for(int i = 0; i < len; ++i){
                tmpVel = animationList[i].getVelocity(timeCount);
                animationList[i].posX += deltaTime * (tmpVel.first * (spacing + blockSize));
                animationList[i].posY += deltaTime * (tmpVel.second * (spacing + blockSize));
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
    int blockSize, spacing;
    SDL_Rect renderRect;
    SDL_Rect blockInImgRect;

    GameState gamestate;

private:
    struct motionPath{
        static const Uint32 animalTimeLength = 200;
        motionPath(int fi, int fj, int toi, int toj, int src_, int tag_)noexcept:
                    fromI{fi}, fromJ{fj}, toI{toi}, toJ{toj},
                    srcNum{src_}, targetNum{tag_}{
            periodicSin = PI / animalTimeLength;
        }
        std::pair<double, double> getVelocity(int timeCount){
            double tmp = std::sin(timeCount * periodicSin) * PI / animalTimeLength;
            return  {tmp * (toJ - fromJ) / 2.0, tmp * (toI - fromI) / 2.0};
        }

        int fromI, fromJ;
        int toI, toJ;
        int targetNum; //the number at toi, toj (i.e. data[toi][toj])
        int srcNum;
        double posX, posY;
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

};

#endif