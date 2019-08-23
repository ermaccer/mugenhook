#include "hooks.h"


void eHooks::Init()
{
	CIniReader ini("");
	bGameModeTagShow = ini.ReadBoolean("Settings", "bGameModeTagShow", 0);
	bGameModeSingleHide = ini.ReadBoolean("Settings", "bGameModeSingleHide", 0);
	bGameModeSimulHide = ini.ReadBoolean("Settings", "bGameModeSimulHide", 0);
	bGameModeTurnsHide = ini.ReadBoolean("Settings", "bGameModeTurnsHide", 0);
	bHookCursorTable = ini.ReadBoolean("Settings", "bHookCursorTable", 0);
	bHookAnimatedPortraits = ini.ReadBoolean("Settings", "bHookAnimatedPortraits", 0);
	bRandomStageConfirmSounds = ini.ReadBoolean("Settings", "bRandomStageConfirmSounds", 0);
	bEnableSelectAnimations = ini.ReadBoolean("Settings", "bEnableSelectAnimations", 0);
    bChangeStrings= ini.ReadBoolean("Settings", "bChangeStrings", 0);
	bDumpCharacterInfo = ini.ReadBoolean("Settings", "bDumpCharacterInfo", 0);
	bDevMode = ini.ReadBoolean("Settings", "bDevMode", 0);
	bDev_DisplayPos = ini.ReadBoolean("Settings", "bDev_DisplayPos", 0);

	iSelectableFighters = ini.ReadInteger("Settings", "iSelectableFighters", 0);

	stage_group = ini.ReadInteger("Settings", "iRandomStageGroup", 0);
	stage_max = ini.ReadInteger("Settings", "iRandomStageRandomMax", 0);


	if (bDevMode)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}

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

	CursorTabMan::Init();

	if (bHookCursorTable)
	{
		CursorTabMan::ReadFile("cfg\\soundAnn.dat");
		Patch<char>(0x406E51, 0xE9);
		Patch<int>(0x406E51 + 1, (int)eHooks::HookCursorSounds - ((int)0x406E51 + 5));
		if (bHookAnimatedPortraits)
		{
			CursorTabMan::AnimatedPortaits::ReadFile("cfg\\animatedPortraits.dat");
			CursorTabMan::AnimatedPortaits::ReadFrameFile("cfg\\frameLoader.dat");
			Patch<char>(0x404CE2, 0xE9);
			Patch<int>(0x404CE2 + 1, (int)CursorTabMan::AnimatedPortaits::HookLoadSprites - ((int)0x404CE2 + 5));
			Patch<int>(0x406FF3 + 1, (int)CursorTabMan::AnimatedPortaits::HookDisplaySprites - ((int)0x406FF3 + 5));
		}

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
	frameTable = std::make_unique<eSprite[]>(MAX_FRAMES);
	printf("CursorTabMan: Allocated %d bytes\n", memSize * sizeof(eCursorEntry));
	printf("Animated Portraits: Allocated %d bytes\n", memSize * sizeof(ePortraitEntry));
	printf("FrameLoader: Allocated %d bytes\n", MAX_FRAMES * sizeof(eSprite));
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


	// gets set to 1 if finished all selection
	int PlayeroneSelected = (*(int*)(cursor_eax + 14592));
	int PlayertwoSelected = (*(int*)(cursor_eax + 14592 + 4));

	// how many times someone selected
	int PlayeroneSelectedTurns = (*(int*)(cursor_eax + 14592 + 36));
	int PlayertwoSelectedTurns = (*(int*)(cursor_eax + 14592 + 36 + 116));

	// was selection made in training
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
	else 
	{
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x344) = select_default_grp2;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x34C) = select_default_sound2;
	}

	if (PlayerTraining == 1  && (CharactersArray[charP1].id >= 0))
	{
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x340) = cursorTable[FoundEntry].snd_group;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x348) = cursorTable[FoundEntry].snd_select;
	}
	else 
	{
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x340) = select_default_grp;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x348) = select_default_sound;
	}


	// we're at select screen anyway
	if (bRandomStageConfirmSounds)
	{
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x36C) = stage_group;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x36C + 4) = rand() % stage_max;
	}

	if (bDevMode)
	{
		if (bDev_DisplayPos)
		{
			printf("Player 1: Row: %d   Column: %d     \r", player1_row, player1_column);
		}

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

				int group_id = 0;
				int group_id2 = 0;
				int column_id = 0;
				int maxframes = 0;
				int frametime = 0;
				int win_grp = 0;
				int win_grp_p2 = 0;
				int win_max = 0;
				int win_frametime = 0;
				int flags = 0;
				sscanf(line, "%d %d %d %d %d %d %d %d %d %d %d", &row_id, &column_id, &group_id, &group_id2, &maxframes, &frametime, &win_grp, &win_grp_p2, &win_max, &win_frametime, &flags);

				// create thing
				ePortraitEntry ent;
				ent.id_row = row_id;
				ent.id_column = column_id;
				ent.group = group_id;
				ent.group_p2 = group_id2;
				ent.frametime = frametime;
				ent.max_frames = maxframes;
				ent.win_group = win_grp;
				ent.win_group_p2 = win_grp_p2;
				ent.win_max_frames = win_max;
				ent.win_frametime = win_frametime;
				ent.flags = flags;

				animTable[lastAnim] = ent;
				lastAnim++;
			}

		}
		printf("CursorTabMan: Animated Portraits: Found %d entities!\n", lastAnim);
		fclose(pFile);
	}
}
void eHooks::CursorTabMan::AnimatedPortaits::ReadFrameFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");
	if (!pFile)
	{
		printf("FrameLoader: ERROR: Could not open %s!\n", file);
	}
	if (pFile)
	{
		printf("FrameLoader:  Reading %s\n", file);
		char line[1536];
		while (fgets(line, sizeof(line), pFile))
		{
			// check if comment
			if (line[0] == ';' || line[0] == '#' || line[0] == '\n')
				continue;

			int group_id = 0;
			if (sscanf(line, "%d", &group_id) == 1)
			{

				int index_id = 0;
				sscanf(line, "%d %d", &group_id, &index_id);

				// create thing
				eSprite spr;
				spr.group = group_id;
				spr.index = index_id;

				frameTable[lastFrame] = spr;
				lastFrame++;
			}

		}
		printf("FrameLoader: Found %d entities!\n", lastFrame);
		fclose(pFile);
	}
}
int eHooks::CursorTabMan::AnimatedPortaits::LoadSprites(int a1, int a2)
{
	return ((int(__cdecl*)(int, int))0x467B30)(a1, a2);
}
int eHooks::CursorTabMan::AnimatedPortaits::HookDisplaySprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{

	Process();
	return  ((int(__cdecl*)(int, int, int, int, int, float, float))0x411C00)(a1, a2, a3, a4, a5, a6, a7);
}
int eHooks::CursorTabMan::AnimatedPortaits::FindFrame(int row, int col)
{
	int iFind = 0;
	for (int i = 0; i < lastFrame; i++)
	{
		if (animTable[i].id_row == row) {
			if (animTable[i].id_column == col)
			{
				iFind = i;
				break;
			}
		}
	}
	return iFind;
}
void __declspec(naked) eHooks::CursorTabMan::AnimatedPortaits::HookLoadSprites()
{

	frameTablePtr = (int)frameTable.get();
	_asm {
		push frameTablePtr
		call 	eHooks::CursorTabMan::AnimatedPortaits::LoadSprites
		pushad
	}
	_asm {
		popad
		jmp sprite_jmp
	}

}
void eHooks::CursorTabMan::AnimatedPortaits::Process()
{

	// gets set to 1 if finished all selection
	int PlayeroneSelected = (*(int*)(cursor_eax + 14592));
	int PlayertwoSelected = (*(int*)(cursor_eax + 14592 + 4));

	// how many times someone selected
	int PlayeroneSelectedTurns = (*(int*)(cursor_eax + 14592 + 36));
	int PlayertwoSelectedTurns = (*(int*)(cursor_eax + 14592 + 36 + 116));

	// was selection made in training
	int PlayerTraining = (*(int*)(cursor_eax + 14864 + 24));



	int player1_row = (*(int*)(cursor_eax + 14864 + 4));
	int player1_column = (*(int*)(cursor_eax + 14864));

	int player2_row = (*(int*)(cursor_eax + 14864 + 0xC0 + 4));
	int player2_column = (*(int*)(cursor_eax + 14864 + 0xC0));


	int frameEntry = CursorTabMan::AnimatedPortaits::FindFrame(player1_row, player1_column);
	int frameEntry_p2 = CursorTabMan::AnimatedPortaits::FindFrame(player2_row, player2_column);

	int frameTime_p1 = animTable[frameEntry].frametime;
	int frameTime_p2 = animTable[frameEntry_p2].frametime;



	if (iFrameCounter_p2 > animTable[frameEntry_p2].max_frames) iFrameCounter_p2 = 0;


	if (!bEnableSelectAnimations)
	{
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x80C) = animTable[frameEntry].group;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x810) = iFrameCounter_p1;
		if (iFrameCounter_p1 > animTable[frameEntry].max_frames) iFrameCounter_p1 = 0;
		if (GetTickCount() - iTickCounter_p1 <= frameTime_p1) return;
		iFrameCounter_p1++;
		iTickCounter_p1 = GetTickCount();


		*(int*)(*(int*)Mugen_ResourcesPointer + 0x810 + 0xD0 + 4) = animTable[frameEntry_p2].group_p2;
		*(int*)(*(int*)Mugen_ResourcesPointer + 0x810 + 0xD0 + 8) = iFrameCounter_p2;
		if (GetTickCount() - iTickCounter_p2 <= frameTime_p2) return;
		iFrameCounter_p2++;
		iTickCounter_p2 = GetTickCount();
	}
	else
	{

		if (!PlayeroneSelected)
		{
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x80C) = animTable[frameEntry].group;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x810) = iFrameCounter_p1;
			if (GetTickCount() - iTickCounter_p1 <= frameTime_p1) return;
			iFrameCounter_p1++;
			iTickCounter_p1 = GetTickCount();
			iWinFrameCounter_p1 = 0;
		}
		else
		{
			frameEntry = CursorTabMan::AnimatedPortaits::FindFrame(player1_row, player1_column);
			if (iWinFrameCounter_p1 > animTable[frameEntry].win_max_frames) iWinFrameCounter_p1 = animTable[frameEntry].win_max_frames;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x80C) = animTable[frameEntry].win_group;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x810) = iWinFrameCounter_p1;
			if (GetTickCount() - iTickCounter_p1 <= animTable[frameEntry].win_frametime) return;
			iWinFrameCounter_p1++;
			iTickCounter_p1 = GetTickCount();
		}

		if (!PlayertwoSelected)
		{
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x810 + 0xD0 + 4) = animTable[frameEntry_p2].group_p2;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x810 + 0xD0 + 8) = iFrameCounter_p2;
			if (GetTickCount() - iTickCounter_p2 <= frameTime_p2) return;
			iFrameCounter_p2++;
			iTickCounter_p2 = GetTickCount();
			iWinFrameCounter_p2 = 0;
		}
		else
		{
			frameEntry_p2 = CursorTabMan::AnimatedPortaits::FindFrame(player2_row, player2_column);
			if (iWinFrameCounter_p2 > animTable[frameEntry_p2].win_max_frames) iWinFrameCounter_p2 = animTable[frameEntry_p2].win_max_frames;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x810 + 0xD0 + 4) = animTable[frameEntry_p2].win_group_p2;
			*(int*)(*(int*)Mugen_ResourcesPointer + 0x810 + 0xD0 + 8) = iWinFrameCounter_p2;
			if (GetTickCount() - iTickCounter_p2 <= animTable[frameEntry_p2].win_frametime) return;
			iWinFrameCounter_p2++;
			iTickCounter_p2 = GetTickCount();
		}

	}

}
