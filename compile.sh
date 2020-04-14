#!/bin/bash
g++ -std=c++14 \
-I/home/maojh/thirdPartCLib/include/ \
main.cpp -o 2048 -lSDL2 -lSDL2_ttf -lSDL2_image

