#pragma once

#include <iostream>

int   RequestSprites(FILE* sff, int* spritesID);
int   DrawSprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
void  ScaleSprites(int ptr, float a1, float a2);
int   SpriteExists(int sprite);

int   LoadSFFFile(char* file);