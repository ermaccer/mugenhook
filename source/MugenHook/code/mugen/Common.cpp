#include "Common.h"
#include "..\mugenhook\base.h"

void DrawScreenMessage(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b)
{
	Call<0x46CE40,const char*, int,int,int,int,char,char,char>(message, a2, a3, a4, a5, r, g, b);
}

void PushDebugMessage(const char * message)
{
	Call<0x40C4A0, const char*>(message);
}
