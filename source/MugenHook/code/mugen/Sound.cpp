#include "Sound.h"
#include "..\mugenhook\base.h"
Sound * LoadSNDFile(const char * name)
{
	return CallAndReturn<Sound*, 0x46EF30, const char*>(name);
}

void PlaySound(Sound * snd, int group, int unknown, int index, float vol)
{
	STDCall<0x444AA0, Sound*, int, int, int, float>(snd, group, unknown, index, vol);
}
