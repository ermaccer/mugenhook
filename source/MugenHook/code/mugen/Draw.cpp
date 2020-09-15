#include "Draw.h"
#include "..\mugenhook\base.h"

int LoadFont(char * file, int param)
{
	return CallAndReturn<int, 0x46D030, char*, int>(file, param);
}

void Draw2DText(char* text, int font, int x, int y, int unk, int unk2, int r, int g, int b)
{
	Call<0x46CE40, char*, int, int, int, int, int, int, int, int>(text, font, x, y, unk, unk2, r, g, b);
}