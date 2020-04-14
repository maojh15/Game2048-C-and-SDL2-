#ifndef GAME_2048_H_
#define GAME_2048_H_
#include<iostream>
#include<mySDL2Tools/MySDLStruct.h>
#include<random>
#include<ctime>
#include<vector>

class game2048{
public:
    game2048(SDL_Renderer *renderer_, const char* imgFilename, SDL_Color backgroundColor,
                SDL_Rect renderRect_, int sizeOfBlock_, int spacing_):
                bgColor{backgroundColor}, renderer{renderer_},
                renderRect{renderRect_}, blockSize{sizeOfBlock_},
                spacing{spacing_}, runFlag{true}{
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
    }

    //direction = 0, 1, 2, 3 correspond to four directions up, right, down, left respectively;
    void moveBlocks(int direction){
        bool movedFlag = false;
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
                                data[0][j] = data[i][j];
                                data[i][j] = 0;
                            }
                            else if(data[temp1][j] == data[i][j]){ // merge
                                ++data[temp1][j];
                                data[i][j] = 0;
                            }
                            else if(temp1 < i - 1){
                                data[temp1 + 1][j] = data[i][j];
                                data[i][j] = 0;
                            }
                        }
                    }
                }
                break;
            case 1: //right
                for(int i = 0; i < maxWidthHeight; ++i){
                    for(int j = maxWidthHeight - 1; j > 0; --j){
                        if(data[i][j] != 0){
                            temp1 = i - 1;
                            while(temp1 >= 0 && data[temp1][j] == 0){
                                //find nearest non-empty object.
                                --temp1;
                            }
                            if(temp1 < 0){ // empty at position temp1==0, j
                                data[0][j] = data[i][j];
                                data[i][j] = 0;
                            }
                            else if(data[temp1][j] == data[i][j]){ // merge
                                ++data[temp1][j];
                                data[i][j] = 0;
                            }
                            else if(temp1 < i - 1){
                                data[temp1 + 1][j] = data[i][j];
                                data[i][j] = 0;
                            }
                        }
                    }
                }
                break;
            case 2: //down
                for(int j = 0; j < maxWidthHeight; ++j){
                    for(int i = maxWidthHeight - 1; i > 0; --i){
                        if(data[i][j] == 0){ // move to empty
                            data[i][j] = data[i-1][j];
                            data[i-1][j] = 0;
                            movedFlag = true;
                        }
                        else if(data[i][j] == data[i-1][j]){ //merge
                            ++data[i][j];
                            data[i-1][j] = 0;
                            movedFlag = true;
                        }
                    }
                }
                break;
            case 3: //left
                for(int i = 0; i < maxWidthHeight; ++i){
                    for(int j = 1; j < maxWidthHeight; ++j){
                        if(data[i][j-1] == 0){ // move to empty
                            data[i][j-1] = data[i][j];
                            data[i][j] = 0;
                            movedFlag = true;
                        }
                        else if(data[i][j] == data[i][j-1]){ //merge
                            ++data[i][j-1];
                            data[i][j] = 0;
                            movedFlag = true;
                        }
                    }
                }
                break;
        }

        if(movedFlag){
            newBlock();
        }
    }

    void newBlock(){
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

        tempNum.clear();
    }

    //used for debug, test the render function
    void test(){
        std::uniform_int_distribution<int> u{0, 11};
        for(int i = 0; i < maxWidthHeight; ++i)
            for(int j = 0; j < maxWidthHeight; ++j)
                data[i][j] = u(randomEngine);

    }

    static const int maxWidthHeight = 4; //4x4 lattice.

    int data[maxWidthHeight][maxWidthHeight];
    SDL_Color bgColor;
    SDL_Renderer *renderer;
    TextureWrap texture;
    int blockSize, spacing;
    SDL_Rect renderRect;
    SDL_Rect blockInImgRect;

    bool runFlag;

private:
    SDL_Rect tempRect;
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<int> randomPos{0, 3};
    std::uniform_int_distribution<int> randomNum{1, 2};
    std::vector<std::pair<int, int> > tempNum;
};

#endif