#include "Sound.h"
#include "..\mugenhook\base.h"
Sound * LoadSNDFile(const char * name)
{
	return CallAndReturn<Sound*, 0x46EF30, const char*>(name);
}

void PlaySound(Sound * snd, int group, int index, int unk, float vol)
{
	((void(__cdecl*)(Sound*, int, int, int, float))0x46ED30)(snd, group, index, unk, vol);
}
