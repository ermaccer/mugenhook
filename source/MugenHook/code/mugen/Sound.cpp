#include "Sound.h"
#include "..\mugenhook\base.h"
#include "System.h"

Sound * LoadSNDFile(const char * name)
{
	return CallAndReturn<Sound*, 0x46EF30, const char*>(name);
}

void PlaySound(Sound * snd, int group, int index, int unk, double vol)
{
	((void(__cdecl*)(Sound*, int, int, int, float))0x46ED30)(snd, group, index, unk, vol);
}

int __declspec(naked) ReloadStageBGM(eStageData* data)
{
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov		eax, data

		mov edx, 0x423580
		call edx

		mov esp, ebp
		pop ebp
		ret
	}

}


void LoadBGM(char * name)
{
	eMugenData* data = eSystem::GetData();
	sprintf(data->StageData.BGMusic, name);

	int result = CallAndReturn<int, 0x470C10, char*>(data->StageData.BGMusic);
	if (result)
		data->RecalculateBGMVolume();
}
