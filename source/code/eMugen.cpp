#include "eMugen.h"
#include "eLog.h"
#include "..\IniReader.h"
#include <iostream>
#include "..\MemoryMgr.h"
#include <filesystem>

eMugenSystem* MugenSystem = new eMugenSystem();

using namespace Memory::VP;

void eMugenManager::Init()
{
	MugenSystem->pMugenResourcesPointer = 0x503388;
	MugenSystem->pMugenCharactersPointer = 0x503394;
	MugenSystem->pMugenDataPointer = 0x5040E8;
	char* motif;
	if (std::experimental::filesystem::exists("data\\mugen.cfg"))
	{
		CIniReader mugenConfig("data\\mugen.cfg");
		motif = mugenConfig.ReadString("Options", "motif", 0);
		if (motif)
		{
			CIniReader system(motif);
			MugenSystem->iRows = system.ReadInteger("Select Info", "rows", 0);
			MugenSystem->iColumns = system.ReadInteger("Select Info", "columns", 0);
			sscanf(system.ReadString("Select Info", "p1.cursor.done.snd", 0), "%d,%d", &MugenSystem->iSoundP1DoneGroup, &MugenSystem->iSoundP1DoneIndex);
			sscanf(system.ReadString("Select Info", "p2.cursor.done.snd", 0), "%d,%d", &MugenSystem->iSoundP2DoneGroup, &MugenSystem->iSoundP2DoneIndex);
			Log->PushMessage(false,"eMugenManager::Init() | Success! Done reading motif data\n");
		}
	}
	else
	{
		Log->PushMessage(false,"eMugenManager::Init() | Failed! Could not open mugen.cfg!\n");
		Log->PushError();
	}
}


int eMugenManager::GetTimer()
{
	return *(int*)(*(int*)MugenSystem->pMugenDataPointer + 0x11E98);
}

int eMugenManager::GetGameFlow()
{
	return *(int*)(*(int*)(0x5040C4));
}

int eMugenManager::GetGameplayMode()
{
	return *(int*)(*(int*)MugenSystem->pMugenDataPointer + 0x5094);
}

int RequestSprites(int param, int sprite_indexes)
{
	return ((int(__cdecl*)(int, int))0x467B30)(param, sprite_indexes);
}

int DrawSprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	return  ((int(__cdecl*)(int, int, int, int, int, float, float))0x411C00)(a1, a2, a3, a4, a5, a6, a7);;
}

void ScaleSprites(int ptr, float a1, float a2)
{
	((void(__cdecl*)(int, float, float))0x412120)(ptr, a1,a2);
}

int DrawScreenMessage(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b)
{
	return  ((int(__cdecl*)(const char*, int, int, int, int, char, char, char))0x46CE40)(message, a2, a3, a4, a5, r, g, b);;
}

void PushDebugMessage(const char* message)
{
	((void(__cdecl*)(const char*))0x40C4A0)(message);
}

void PlaySound(int a1, int a2, int a3, int a4, double a5)
{
	((void(__cdecl*)(int, int, int, int, double))0x444AA0)(a2, a2, a3, a4, a5);
}

int GetButtonID(int ptr)
{
	return ((int(__thiscall*)(int))0x478900)(ptr);
}

void DrawTextFont(const char * text, int x, int y, int unk)
{
	((void(__cdecl*)(const char*,int,int,int))0x45FD00)("FONT DRAW TEST",x,y,unk);
}

int HookDrawScreenMessage(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b)
{
	char temp[256];
	sprintf(temp, "%s | MugenHook %d.%d.%d by ermaccer", message, 0, 5, 1);
	return DrawScreenMessage(temp, a2, a3, a4, a5, r, g, b);
}

void HookPushDebugMessage(const char * message)
{
	printf(message);
}

void HookSoundPlayback(int a1, int a2, int a3, int a4, double a5)
{
	printf("A1: %d A2: %d A3: %d A4: 0x%X A5: %f\n", a1, a2, a3, a4, a5);
	//PlaySound(a1, a2, a3, a4, a5);
}

