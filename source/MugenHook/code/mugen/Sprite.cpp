#include "..\mugenhook\base.h"

#include "Sprite.h"

int RequestSprites(FILE * sff, int * spritesID)
{
	return CallAndReturn<int, 0x467B30, FILE*, int*>(sff, spritesID);
}

int DrawSprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	return CallAndReturn<int, 0x411C00, int, int, int, int, int, float, float>(a1, a2, a3, a4, a5, a6, a7);
}

void ScaleSprites(int ptr, float a1, float a2)
{
	Call<0x412120, int, float, float>(ptr, a1, a2);
}

int SpriteExists(int sprite)
{
	return CallAndReturn<int, 0x466840, int>(sprite);
}

int LoadSFFFile(char * file)
{
	return CallAndReturn<int, 0x467510, const char*>(file);
}
