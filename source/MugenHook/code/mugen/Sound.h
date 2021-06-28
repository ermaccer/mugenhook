#pragma once

typedef int Sound;

struct SoundData {
	bool   m_bIsCached;
	bool   m_bReserved1;
	bool   m_bReserved2;
	bool   m_bReserved3;
	Sound* m_pSoundData;
};

Sound* LoadSNDFile(const char* name);
void PlaySound(Sound* snd, int group, int index, int unk, double vol);

