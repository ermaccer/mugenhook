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
int eSystem::p1_cursor_startcell[2];
int eSystem::p2_cursor_startcell[2];
float eSystem::p1_face_offset[2];
float eSystem::p2_face_offset[2];
pushstart_settings eSystem::pushstart_set = {};

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

	p1_cursor_startcell[0] = 0;
	p1_cursor_startcell[1] = 0;

	p2_cursor_startcell[0] = 0;
	p2_cursor_startcell[1] = 0;

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

			sscanf(system.ReadString("Select Info", "p1.cursor.startcell", 0), "%d,%d", &p1_cursor_startcell[0], &p1_cursor_startcell[1]);

			sscanf(system.ReadString("Select Info", "p2.cursor.startcell", 0), "%d,%d", &p2_cursor_startcell[0], &p2_cursor_startcell[1]);
			// save og values
			sscanf(system.ReadString("Select Info", "p1.face.offset", 0), "%f,%f", &p1_face_offset[0], &p1_face_offset[1]);
			sscanf(system.ReadString("Select Info", "p2.face.offset", 0), "%f,%f", &p2_face_offset[0], &p2_face_offset[1]);


			sscanf(system.ReadString("Select Info", "p1.cursor.done.snd", 0), "%d,%d", &iSoundP1DoneGroup, &iSoundP1DoneIndex);
			sscanf(system.ReadString("Select Info", "p2.cursor.done.snd", 0), "%d,%d", &iSoundP2DoneGroup, &iSoundP2DoneIndex);
			sscanf(system.ReadString("Select Info", "portrait.spr", 0), "%d,%d", &iPortraitGroup, &iPortraitIndex);

			pushstart_set.active = system.ReadInteger("Select Info", "pushstart.active", 0);
			sprintf(pushstart_set.text, system.ReadString("Select Info", "pushstart.text", 0));
			sprintf(pushstart_set.font, system.ReadString("Select Info", "pushstart.font", "font/F-4X6.fnt"));

			for (int i = 0; i < 512; i++)
			{
				if (pushstart_set.text[i] == 0x5C)
				{
					printf("patching\n");
					if (pushstart_set.text[i + 1] == 0x6E)
					{
						pushstart_set.text[i] = 0xD;
						pushstart_set.text[i + 1] = 0xA;
					}
				}
			}


			pushstart_set.color_r = system.ReadInteger("Select Info", "pushstart.color.r", 0);
			pushstart_set.color_g = system.ReadInteger("Select Info", "pushstart.color.g", 0);
			pushstart_set.color_b = system.ReadInteger("Select Info", "pushstart.color.b", 0);

			pushstart_set.scale_x = system.ReadFloat("Select Info", "pushstart.scale.x", 0);
			pushstart_set.scale_y = system.ReadFloat("Select Info", "pushstart.scale.y", 0);

			pushstart_set.p1_x = system.ReadInteger("Select Info", "pushstart.p1.x", 0);
			pushstart_set.p1_y = system.ReadInteger("Select Info", "pushstart.p1.y", 0);

			pushstart_set.p2_x = system.ReadInteger("Select Info", "pushstart.p2.x", 0);
			pushstart_set.p2_y = system.ReadInteger("Select Info", "pushstart.p2.y", 0);

			pushstart_set.p1_align = system.ReadInteger("Select Info", "pushstart.p1.align", 0);
			pushstart_set.p2_align = system.ReadInteger("Select Info", "pushstart.p2.align", 0);

			pushstart_set.group = system.ReadInteger("Select Info", "pushstart.snd.group", -1);
			pushstart_set.index = system.ReadInteger("Select Info", "pushstart.snd.index", -1);

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

void eSystem::SetGameFlow(eGameFlowType flow)
{
	*(int*)(*(int*)(0x5040C4)) = flow;
}

void eSystem::SetGameplayMode(eGameplayModes mode)
{
	if (*(int*)pMugenDataPointer)
		*(int*)(*(int*)pMugenDataPointer + 0x5094) = mode;
}

void eSystem::ClearScreenparams(eSelectScreenParams * params)
{
	for (int i = 0; i < 800; i++)
		params->data[i] = 0x00;
}

void  __declspec(naked) eSystem::SetScreenParams(eSelectScreenParams * params, int gameMode, int unk)
{
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov eax, unk
		mov edx, gameMode
		mov ecx, params

		mov esi, 0x408B20
		call esi

		mov esp, ebp
		pop ebp
		ret
	}
}
