#include "eMugen.h"
#include "..\IniReader.h"
#include <iostream>
#include <filesystem>

int iRows, iColumns;
int iSoundP1DoneGroup, iSoundP1DoneIndex;
int iSoundP2DoneGroup, iSoundP2DoneIndex;
int pMugenResourcesPointer = 0x503388;
int pMugenCharactersPointer = 0x503394;
int pMugenDataPointer = 0x5040E8;

void eMugenManager::Init()
{
	char* motif;
	if (std::experimental::filesystem::exists("data\\mugen.cfg"))
	{
		CIniReader mugenConfig("data\\mugen.cfg");
		motif = mugenConfig.ReadString("Options", "motif", 0);
		if (motif)
		{
			CIniReader system(motif);
			iRows = system.ReadInteger("Select Info", "rows", 0);
			iColumns = system.ReadInteger("Select Info", "columns", 0);
			sscanf(system.ReadString("Select Info", "p1.cursor.done.snd", 0), "%d,%d", &iSoundP1DoneGroup, &iSoundP1DoneIndex);
			sscanf(system.ReadString("Select Info", "p2.cursor.done.snd", 0), "%d,%d", &iSoundP2DoneGroup, &iSoundP2DoneIndex);
			printf("eMugenManager::Init() | Sucess! Done reading motif data\n");
		}
	}
	else
	{
		printf("eMugenManager::Init() | Failed! Could not open mugen.cfg!\n");
	}
}

int eMugenManager::GetRows()
{
	return iRows;
}

int eMugenManager::GetColumns()
{
	return iColumns;
}

int eMugenManager::GetSoundDoneGroup(int player)
{
	if (player == 2)
		return iSoundP2DoneGroup;
	else
		return iSoundP1DoneGroup;
}

int eMugenManager::GetSoundDoneIndex(int player)
{
	if (player == 2)
		return iSoundP2DoneIndex;
	else
		return iSoundP1DoneIndex;
}

int eMugenManager::GetResourcesPointer()
{
	return pMugenResourcesPointer;
}

int eMugenManager::GetDataPointer()
{
	return pMugenDataPointer;
}

int eMugenManager::GetCharactersPointer()
{
	return pMugenCharactersPointer;
}

int eMugenManager::GetTimer()
{
	return *(int*)(*(int*)eMugenManager::GetDataPointer() + 0x11E98);
}

int RequestSprites(int param, int sprite_indexes)
{
	return ((int(__cdecl*)(int, int))0x467B30)(param, sprite_indexes);
}

int DrawSprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	return  ((int(__cdecl*)(int, int, int, int, int, float, float))0x411C00)(a1, a2, a3, a4, a5, a6, a7);;
}

int DrawScreenMessage(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b)
{
	return  ((int(__cdecl*)(const char*, int,int,int,int,char,char,char))0x46CE40)(message,a2, a3, a4, a5, r, g, b);;
}

void PushDebugMessage(const char* message)
{
	((void(__cdecl*)(const char*))0x40C4A0)(message);
}

void PlaySound(int a1, int a2, int a3, int a4, double a5)
{
	((void(__cdecl*)(int,int,int,int,double))0x444AA0)(a2,a2,a3,a4,a5);
}

int GetButtonID(int ptr)
{

	return ((int(__thiscall*)(int))0x478900)(ptr);
}

int HookDrawScreenMessage(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b)
{
	char temp[256];
	sprintf(temp, "%s | MugenHook by ermaccer", message);
	return DrawScreenMessage(temp,a2,a3,a4,a5,r,g,b);
}

void HookPushDebugMessage(const char * message)
{
	printf(message);
}

void HookSoundPlayback(int a1, int a2, int a3, int a4, double a5)
{
	printf("A1: %d A2: %d A3: %d A4: 0x%X A5: %f\n",a1,a2,a3,a4,a5);
	//PlaySound(a1, a2, a3, a4, a5);
}
