#include "hooks.h"


void eHooks::Init()
{
	CIniReader ini("");
	bGameModeTagShow = ini.ReadBoolean("Settings", "bGameModeTagShow", 0);
	bGameModeSingleHide = ini.ReadBoolean("Settings", "bGameModeSingleHide", 0);
	bGameModeSimulHide = ini.ReadBoolean("Settings", "bGameModeSimulHide", 0);
	bGameModeTurnsHide = ini.ReadBoolean("Settings", "bGameModeTurnsHide", 0);
	bHookCursorTable = ini.ReadBoolean("Settings", "bHookCursorTable", 0);
	bRandomStageConfirmSounds = ini.ReadBoolean("Settings", "bRandomStageConfirmSounds", 0);
    bChangeStrings= ini.ReadBoolean("Settings", "bChangeStrings", 0);
	bDumpCharacterInfo = ini.ReadBoolean("Settings", "bDumpCharacterInfo", 0);

	iSelectableFighters = ini.ReadInteger("Settings", "iSelectableFighters", 0);

	stage_group = ini.ReadInteger("Settings", "iRandomStageGroup", 0);
	stage_max = ini.ReadInteger("Settings", "iRandomStageRandomMax", 0);

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
		Patch<const char*>(0x41A591 + 1, ini.ReadString("Strings", "szAppTitle ", "M.U.G.E.N"));
	}

	if (iSelectableFighters)
	Patch<int>(0x4063F0, iSelectableFighters);

	if (bHookCursorTable)
	{
		CursorTabMan::Init();
		CursorTabMan::ReadFile("cfg\\soundAnn.dat");
		//CursorTabMan::AnimatedPortaits::ReadFile("cfg\\animatedPortraits.dat");
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
	CursorTabMan::ProcessSelectScreen();
	_asm {
		popad
		jmp cursor_jmp
	}
}

void eHooks::PrintCharacterNames()
{
	MugenCharacter* CharactersArray = *(MugenCharacter**)0x503394;
	std::ofstream oFile("characters.txt", std::ofstream::binary);

	if (CharactersArray)
	{
		for (int i = 0; i < row * column; i++)
		{
			std::string strName(CharactersArray[i].name, strlen(CharactersArray[i].name));
			oFile << "Character: " << strName << " ID:" << std::to_string(CharactersArray[i].id) <<  "\n Folder: " << CharactersArray[i].foldername << std::endl;
		}
		oFile.close();
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
			row = system.ReadInteger("Select Info", "rows",0);
			column = system.ReadInteger("Select Info", "columns", 0);
			sscanf(system.ReadString("Select Info", "p1.cursor.done.snd", 0), "%d,%d", &select_default_grp, &select_default_sound);
			sscanf(system.ReadString("Select Info", "p2.cursor.done.snd", 0), "%d,%d", &select_default_grp2, &select_default_sound2);
			memSize = row * column;
		}
	}
	else
		MessageBoxA(0, "Could not locate mugen.cfg!", "ERROR", MB_ICONEXCLAMATION);


	cursorTable = std::make_unique<eCursorEntry[]>(memSize);
	animTable = std::make_unique<ePortraitEntry[]>(memSize);
	printf("CursorTabMan: Init allocated %d bytes\n", memSize * sizeof(eCursorEntry));
	printf("Animated Portraits: Init allocated %d bytes\n", memSize * sizeof(ePortraitEntry));
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
		printf("CursorTabMan: Found %d entities!\n", lastEntry);
		fclose(pFile);
	}

}

void eHooks::CursorTabMan::ProcessSelectScreen()
{
	FoundEntry = CursorTabMan::FindSound(*(int*)(cursor_eax + 14864 + 16), *(int*)(cursor_eax + 14864));
	FoundEntryp2 = CursorTabMan::FindSound(*(int*)(cursor_eax + 14864 + 0xC0 + 16), *(int*)(cursor_eax + 14864 + 0xC0));


	int PlayeroneSelected = (*(int*)(cursor_eax + 14592));
	int PlayeroneSelectedTurns = (*(int*)(cursor_eax + 14592 + 36));
	int PlayertwoSelected = (*(int*)(cursor_eax + 14592 + 4));
	int PlayertwoSelectedTurns = (*(int*)(cursor_eax + 14592 + 36 + 116));
	int PlayerTraining = (*(int*)(cursor_eax + 14864 + 24));



	int player1_row = (*(int*)(cursor_eax + 14864 + 4));
	int player1_column = (*(int*)(cursor_eax + 14864));

	int player2_row = (*(int*)(cursor_eax + 14864 + 0xC0 + 4));
	int player2_column = (*(int*)(cursor_eax + 14864 + 0xC0));


	MugenCharacter* CharactersArray = *(MugenCharacter**)0x503394;


	if (GetAsyncKeyState(VK_F5) && bDumpCharacterInfo)
		PrintCharacterNames();

	// fix for repeating turns/training/watch sounds
	// -2 = random  -1 = empty cell
	
	int charP2 = (player2_column + (player2_row * row) + player2_row);
	int charP1 = (player1_column + (player1_row * row) + player1_row);


	if (PlayertwoSelectedTurns == 0  && (CharactersArray[charP2].id >= 0))
	{
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x344) = cursorTable[FoundEntryp2].snd_groupp2;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x34C) = cursorTable[FoundEntryp2].snd_selectp2;
	}
	else {
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x344) = select_default_grp2;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x34C) = select_default_sound2;
	}

	if (PlayerTraining == 1  && (CharactersArray[charP1].id >= 0))
	{
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x340) = cursorTable[FoundEntry].snd_group;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x348) = cursorTable[FoundEntry].snd_select;
	}
	else {
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x340) = select_default_grp;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x348) = select_default_sound;
	}



	// we're at select screen anyway
	if (bRandomStageConfirmSounds)
	{
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x36C) = stage_group;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x36C + 4) = rand() % stage_max;
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

void eHooks::CursorTabMan::AnimatedPortaits::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");
	if (!pFile)
	{
		printf("CursorTabMan: Animated Portraits: ERROR: Could not open %s!\n", file);
	}
	if (pFile)
	{
		printf("CursorTabMan: Animated Portraits: Reading %s\n", file);
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
				int maxframes = 0;
				int frametime = 0;
				sscanf(line, "%d %d %d %d", &row_id, &column_id, &maxframes, &frametime);

				// create thing
				ePortraitEntry ent;
				ent.id_row = row_id;
				ent.id_column = column_id;
				ent.frametime = frametime;
				ent.max_frames = maxframes;

				animTable[lastAnim] = ent;
				lastAnim++;
			}

		}
		printf("CursorTabMan: Animated Portraits: Found %d entities!\n", lastAnim);
		fclose(pFile);
	}
}

int eHooks::CursorTabMan::AnimatedPortaits::DisplaySprites(int a1, int a2, int a3, int a4, int a5, float x, float y)
{
	// a1 - unk
	// a2 - graphics pointer
	// a3 - sprite data pointer
	// a4 - scale pointer
	// a5 - ?
	// a6

	int test = ((int(__cdecl*)(int, int, int, int, int, float, float))0x411C00)(a1, a2, a3, a4, a5, x, y);
	//printf("%d %X %X %X\n", test, a1, a2, a3);
	//return  ((int(__cdecl*)(int, int, int, int, int, float, float))0x411C00)(a1, a2, a3, a4,a5,x ,y);
	return test;
}

int eHooks::CursorTabMan::AnimatedPortaits::LoadSprites(int a1, int a2)
{
		return ((int(__cdecl*)(int, int))0x467B30)(a1, a2);

}
