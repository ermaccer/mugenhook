#include "hooks.h"


void eHooks::Init()
{
	CIniReader ini("mugenhook.ini");
	bGameModeTagShow = ini.ReadBoolean("Settings", "bGameModeTagShow", 0);
	bGameModeSingleHide = ini.ReadBoolean("Settings", "bGameModeSingleHide", 0);
	bGameModeSimulHide = ini.ReadBoolean("Settings", "bGameModeSimulHide", 0);
	bGameModeTurnsHide = ini.ReadBoolean("Settings", "bGameModeTurnsHide", 0);
	bHookCursorTable = ini.ReadBoolean("Settings", "bHookCursorTable", 0);
    bChangeStrings= ini.ReadBoolean("Settings", "bChangeStrings", 0);

	if (bChangeStrings)
	{
		Patch<const char*>(0x40ADD8 + 3, ini.ReadString("Strings", "szGameModeSingle", "Single"));
		Patch<const char*>(0x40AE04 + 3, ini.ReadString("Strings", "szGameModeSimul", "Simul"));
		Patch<const char*>(0x40AE5F + 3, ini.ReadString("Strings", "szGameModeTurns", "Turns"));
		Patch<const char*>(0x40AE30 + 3, ini.ReadString("Strings", "szGameModeTag", "Tag"));
		Patch<const char*>(0x42BD3B + 7, ini.ReadString("Strings","szKeyA","A"));
		Patch<const char*>(0x42BD46 + 7, ini.ReadString("Strings","szKeyB","B"));
		Patch<const char*>(0x42BD51 + 7, ini.ReadString("Strings","szKeyC","C"));
		Patch<const char*>(0x42BD5C + 7, ini.ReadString("Strings","szKeyX","X"));
		Patch<const char*>(0x42BD67 + 7, ini.ReadString("Strings","szKeyY","Y"));
		Patch<const char*>(0x42BD72 + 7, ini.ReadString("Strings","szKeyZ","Z"));
		Patch<const char*>(0x42BD7D + 7, ini.ReadString("Strings","szKeyStart","Start"));
		Patch<const char*>(0x407E43 + 1, ini.ReadString("Strings", "szSelectStageRandom", "Stage: Random"));
		Patch<const char*>(0x407E73 + 1, ini.ReadString("Strings", "szSelectStage", "Stage %i: %s"));
	}

	if (bHookCursorTable)
	{
		eHooks::CursorTabMan::Init();
		eHooks::CursorTabMan::ReadFile("cfg\\soundAnn.dat");
		Patch<char>(0x406E51, 0xE9);
		Patch<int>(0x406E51 + 1, (int)eHooks::HookCursorSounds - ((int)0x406E51 + 5));
	}
	if (bGameModeTagShow)    Patch<char>(0x40AE10, 6);
	if (bGameModeSimulHide)  Patch<char>(0x40ADE4, 4);
	if (bGameModeSingleHide) Patch<char>(0x40ADC2, 4);
	if (bGameModeTurnsHide)  Patch<char>(0x40AE3C, 4);
}

void __declspec(naked) eHooks::HookCursorSounds()
{
	_asm
	{
		mov cursor_eax, eax
		add eax, 14864
		pushad
	}
	// C0 p2
	FoundEntry = CursorTabMan::FindSound(*(int*)(cursor_eax + 14864 + 16), *(int*)(cursor_eax + 14864));
	FoundEntryp2 = CursorTabMan::FindSound(*(int*)(cursor_eax + 14864 + 0xC0 + 16), *(int*)(cursor_eax + 14864 + 0xC0));
	// p1.cursor.done.snd, group
	*(int*)(*(int*)Mugen_ResourcesPointer + 0x340) = cursorTable[FoundEntry].snd_group;
	// p1.cursor.done.snd, value
	*(int*)(*(int*)Mugen_ResourcesPointer + 0x348) = cursorTable[FoundEntry].snd_select;
	// p1.cursor.done.snd, group
	*(int*)(*(int*)Mugen_ResourcesPointer + 0x344) = cursorTable[FoundEntryp2].snd_groupp2;
	// p1.cursor.done.snd, value
	*(int*)(*(int*)Mugen_ResourcesPointer + 0x34C) = cursorTable[FoundEntryp2].snd_selectp2;
	_asm {
		popad
		jmp cursor_jmp
	}
}


void eHooks::CursorTabMan::Init()
{
	int memSize;
	if (std::experimental::filesystem::exists("data\\mugen.cfg"))
	{
		// get system.def
		CIniReader reader("data\\mugen.cfg");
		char* line = reader.ReadString("Options", "motif",0);
		if (line)
		{
			CIniReader system(line);
			int row = system.ReadInteger("Select Info", "rows",0);
			int column = system.ReadInteger("Select Info", "columns", 0);

			memSize = row * column;
		}
	}
	else
		MessageBoxA(0, "Could not locate mugen.cfg!", "ERROR", MB_ICONEXCLAMATION);


	cursorTable = std::make_unique<eCursorEntry[]>(memSize);
	printf("CursorTabMan: Init, allocated %d\n", memSize * sizeof(eCursorEntry));
}

void eHooks::CursorTabMan::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");
	if (!pFile)
	{
		printf("CursorTabMan: ERROR: Could not open %s!\n",file);
	}
	if (pFile)
	{
		printf("CursorTabMan: Reading %s\n", file);
		char line[1536];
		while (fgets(line, sizeof(line), pFile))
		{
			// check if comment
			if (line[0] == ';' || line[0] == '#' || line[0] == '\n')
				continue;

			int row_id = 0;
			if (sscanf(line, "%d", &row_id) == 1)
			{

				int column_id = 0;
				int snd_group = 0;
				int snd_value = 0;
				int snd_group_p2 = 0;
				int snd_value_p2 = 0;
				sscanf(line, "%d %d %d %d %d %d", &row_id, &column_id, &snd_group, &snd_value, &snd_group_p2, &snd_value_p2);

				// create thing
				eCursorEntry ent;

				ent.id_column = column_id;
				ent.id_row = row_id;
				ent.snd_group = snd_group;
				ent.snd_select = snd_value;
				ent.snd_groupp2 = snd_group_p2;
				ent.snd_selectp2 = snd_value_p2;

				cursorTable[lastEntry] = ent;
				lastEntry++;
			}

		}
		printf("CursorTabMan: Found %d entities!\n", lastEntry + 1);
		fclose(pFile);
	}
}

int eHooks::CursorTabMan::FindSound(int row, int col)
{
	int iFind = 0;
	for (int i = 0; i < lastEntry; i++)
	{
		if (cursorTable[i].id_row == row) {
			if (cursorTable[i].id_column == col)
			{
				iFind = i;
				break;
			}
		}
	}
	return iFind;
}
