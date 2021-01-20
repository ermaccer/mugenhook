#pragma once

typedef int Sound;

Sound* LoadSNDFile(const char* name);
void   PlaySound(Sound* snd, int group, int unknown, int index, float vol);