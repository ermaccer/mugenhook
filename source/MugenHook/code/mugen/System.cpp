#include "System.h"
#include <filesystem>
#include "..\core\eLog.h"
#include "..\mugenhook\base.h"
#include "..\mugenhook\eSelectTimer.h"
#include "..\..\IniReader.h"
#include <shellapi.h>

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
screentimer_settings eSystem::screentimer = {};

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


		int argc;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

		motif = mugenConfig.ReadString("Options", "motif", 0);

		if (argv)
		{
			for (int i = 0; i < argc; i++)
			{
				if (wcscmp(argv[i], L"-r") == 0)
				{
					std::wstring wstr = argv[i + 1];
					std::string str("", wstr.length());
					std::copy(wstr.begin(), wstr.end(), str.begin());

					std::string motif_str = "data\\";
					motif_str += str;
					motif = (char*)motif_str.c_str();
				}
			}
		}

		LocalFree(argv);

		eLog::PushMessage(__FUNCTION__, "Loading %s\n", motif);

		if (motif)
		{
			CIniReader system(motif);
			iRows = system.ReadInteger("Select Info", "rows", 0);
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

			ConvertNewLine(pushstart_set.text, strlen(pushstart_set.text));

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

			screentimer.active = system.ReadInteger("Select Info", "screentimer.active", 1);
			screentimer.amount = system.ReadInteger("Select Info", "screentimer.amount", 10);
			screentimer.ticks_per_sec = system.ReadInteger("Select Info", "screentimer.ticks", 60);
			screentimer.num0s = system.ReadInteger("Select Info", "screentimer.zeros", 0);

			sprintf(screentimer.format, system.ReadString("Select Info", "screentimer.text", 0));
			ConvertNewLine(screentimer.format, strlen(screentimer.format));
			sprintf(screentimer.font, system.ReadString("Select Info", "screentimer.font", "font/F-4X6.fnt"));

			screentimer.color_r = system.ReadInteger("Select Info", "screentimer.color.r", 0);
			screentimer.color_g = system.ReadInteger("Select Info", "screentimer.color.g", 0);
			screentimer.color_b = system.ReadInteger("Select Info", "screentimer.color.b", 0);

			screentimer.scale_x = system.ReadFloat("Select Info", "screentimer.scale.x", 0);
			screentimer.scale_y = system.ReadFloat("Select Info", "screentimer.scale.y", 0);

			screentimer.x = system.ReadInteger("Select Info", "screentimer.x", 0);
			screentimer.y = system.ReadInteger("Select Info", "screentimer.y", 0);

			screentimer.text_align = system.ReadInteger("Select Info", "screentimer.align", 0);

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
	if (*(int*)(0x5040C4))
		return *(int*)(*(int*)(0x5040C4));
	else
		return 0;
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

void eSystem::SetRoundTime(int value)
{
	*(int*)(*(int*)eSystem::pMugenDataPointer + 0x12768) = value;
}

int eSystem::GetRoundTime()
{
	return *(int*)(*(int*)eSystem::pMugenDataPointer + 0x12768);
}

eMugenData * eSystem::GetData()
{
	return *(eMugenData**)eSystem::pMugenDataPointer;
}

char * eSystem::GetDirectory()
{
	if (!eSystem::pMugenDataPointer)
		return nullptr;

	eMugenData* data = *(eMugenData**)eSystem::pMugenDataPointer;
	return data->GameFolder;
}

char* CNS_ReadValue(int ini, const char * name)
{
	return CallAndReturn<char*, 0x46A5E0, int, const char*>(ini, name);
}

bool CNS_StoreValue(char * line, int dst, int buff, int unk, int type)
{
	return CallAndReturn<bool, 0x4023D0, char*, int, int, int, int>(line, dst, buff, unk, type);
}

int CNS_RecallValue(int proc, int from, int type)
{
	return CallAndReturn<int, 0x4028C0, int, int, int>(proc, from, type);
}

int GetCharacterIDFromSprite(int sprite)
{
	int id = -1;
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
	for (int i = 0; i < eSystem::GetCharactersAmount(); i++)
	{
		if (CharactersArray[i].SpritePointer == sprite)
		{
			id = CharactersArray[i].ID;
			break;
		}
	}
	return id;
}

void SystemError(const char * text)
{
	Call<0x40C140, const char*>(text);
}

void ConvertNewLine(char * text, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (text[i] == 0x5C)
		{
			if (text[i + 1] == 0x6E)
			{
				text[i] = 0xD;
				text[i + 1] = 0xA;
			}
		}
	}
}
