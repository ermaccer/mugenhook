#include "Data.h"
#include "System.h"
#include <iostream>
#include "../mugenhook/base.h"

int eMugenData::GetTeamCharactersAmount(int team)
{
	int teamData = ((int)this + 0x50A0);
	if (teamData)
	{
		switch (team)
		{
		case TEAM_1:
			return *(int*)(teamData + 8);
			break;
		case TEAM_2:
			return *(int*)(teamData + 0x1ADC + 8);
			break;
		default:
			break;
		}
	}
	return 0;
}

int eMugenData::GetMatch()
{
	return *(int*)((int)this + 0x12820);
}

int eMugenData::GetTotalMatches()
{
	return *(int*)((int)this + 0x12824);
}

int eMugenData::GetRound()
{
	return *(int*)((int)this + 0x12728);
}

int eMugenData::GetWinningSide()
{
	return *(int*)((int)this + 0x12758);
}

eMugenCharacter* eMugenData::GetCharacter(int id)
{
	int field_12474 = *(int*)((int)this + 0x12474);
	int numObjects = *(int*)((int)this + 0x12478);
	eMugenCharacter* character = nullptr;

	if (id >= field_12474 && id <= numObjects)
	{
		int object_ptr = ((int)this + 0x12274 + (4 * id));
		if (object_ptr)
			character = *(eMugenCharacter**)(object_ptr);
	}
	return character;
}

eMugenCharacter* eMugenData::GetCharacter(int team, int slot)
{
	return GetCharacter(team + slot * 2);
}

int eMugenData::GetIDFromObject(int object)
{
	int field_12474 = *(int*)((int)this + 0x12474);
	int numObjects = *(int*)((int)this + 0x12478);
	eMugenCharacter* character = nullptr;

	for (int i = 0; i < numObjects; i++)
	{
		int object_ptr = ((int)this + 0x12274 + (4 * i));
		if (object_ptr == object)
			return i - 1;
	}
	return 0;
}

void eMugenData::PrintAllCharacters()
{
	
}

void eMugenData::RecalculateBGMVolume()
{
	double masterVolume = *(double*)((int)this + 0x11E80);
	double bgmVolume = *(double*)((int)this + 0x11E90);

	double targetVolume = masterVolume * bgmVolume / 100.0;
	Call<0x470820, double>(targetVolume);
}

void __declspec(naked) LoadStage(eStageData* data)
{
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov		ecx, data
		push ecx 
		mov edx, 0x422960
		call edx

		mov		eax, data
		mov edx, 0x423580
		call edx

		mov esp, ebp
		pop ebp
		ret
	}
}
