#include "System.h"
#include <filesystem>
#include "..\core\eLog.h"
#include "..\mugenhook\base.h"

int eSystem::iRows;
int eSystem::iColumns;
int eSystem::iSoundP1DoneGroup;
int eSystem::iSoundP1DoneIndex;

int eSystem::iSoundP2DoneGroup;
int eSystem::iSoundP2DoneIndex;

int eSystem::iPortraitGroup;
int eSystem::iPortraitIndex;

int eSystem::pMugenResourcesPointer;
int eSystem::pMugenCharactersPointer;
int eSystem::pMugenDataPointer;

float eSystem::p1_face_offset[2];
float eSystem::p2_face_offset[2];

void eSystem::Init()
{
	iRows = 0;
	iColumns = 0;
	iSoundP1DoneGroup = 0;
	iSoundP1DoneIndex = 0;

	iSoundP2DoneGroup = 0;
	iSoundP2DoneIndex = 0;

	iPortraitGroup = 0;
	iPortraitIndex = 0;

	p1_face_offset[0] = 0;
	p1_face_offset[1] = 0;

	p2_face_offset[0] = 0;
	p2_face_offset[1] = 0;

	pMugenResourcesPointer = 0x503388;
	pMugenCharactersPointer = 0x503394;
	pMugenDataPointer = 0x5040E8;

	char* motif;
	if (std::filesystem::exists("data\\mugen.cfg"))
	{
		CIniReader mugenConfig("data\\mugen.cfg");
		motif = mugenConfig.ReadString("Options", "motif", 0);
		if (motif)
		{
			CIniReader system(motif);
			iRows =    system.ReadInteger("Select Info", "rows", 0);
			iColumns = system.ReadInteger("Select Info", "columns", 0);

			// save og values
			sscanf(system.ReadString("Select Info", "p1.face.offset", 0), "%f,%f", &p1_face_offset[0], &p1_face_offset[1]);
			sscanf(system.ReadString("Select Info", "p2.face.offset", 0), "%f,%f", &p2_face_offset[0], &p2_face_offset[1]);


			sscanf(system.ReadString("Select Info", "p1.cursor.done.snd", 0), "%d,%d", &iSoundP1DoneGroup, &iSoundP1DoneIndex);
			sscanf(system.ReadString("Select Info", "p2.cursor.done.snd", 0), "%d,%d", &iSoundP2DoneGroup, &iSoundP2DoneIndex);
			sscanf(system.ReadString("Select Info", "portrait.spr", 0), "%d,%d", &iPortraitGroup, &iPortraitIndex);

			eLog::PushMessage(__FUNCTION__, "Success! Done reading motif data\n");
		}
	}
	else
	{
		eLog::PushMessage(__FUNCTION__, "Failed! Could not open mugen.cfg!\n");
		eLog::PushError();
	}
}

Sound * eSystem::GetSystemSND()
{
	return *(Sound**)(*(int*)eSystem::pMugenDataPointer + 0x112C0);
}

int eSystem::GetGameFlow()
{
	return *(int*)(*(int*)(0x5040C4));
}

int eSystem::GetGameplayMode()
{
	if (*(int*)pMugenDataPointer)
		return *(int*)(*(int*)pMugenDataPointer + 0x5094);
	else
		return 0;
}

int eSystem::GetTimer()
{
	if (*(int*)pMugenDataPointer)
		return *(int*)(*(int*)pMugenDataPointer + 0x11E98);
	else
		return 0;
}

int eSystem::GetCharactersAmount()
{
	return iRows * iColumns;
}
