#!/bin/bash
g++ -g -std=c++14 \
-I/home/maojh/thirdPartCLib/include/ \
main.cpp game2048.cpp -o 2048 -lSDL2 -lSDL2_ttf -lSDL2_image -llua

