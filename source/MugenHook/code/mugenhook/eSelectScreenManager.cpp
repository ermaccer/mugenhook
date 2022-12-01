#include "eSelectScreenManager.h"
#include "eCustomCursorManager.h"
#include "..\mugen\System.h"
#include "..\core\eCursor.h"
#include "..\core\eLog.h"
#include "base.h"
#include "..\core\eSettingsManager.h"
#include "..\core\eInputManager.h"
#include "eMagicBoxes.h"
#include "eAnimatedIcons.h"
#include <iostream>
#include <fstream>
#include "eMenuManager.h"
#include "eStageAnnouncer.h"
#include "..\mugen\Sound.h"
#include "..\mugen\Sprite.h"
#include "..\mugen\Common.h"
#include "..\mugen\Draw.h"
#include "eAnimatedPortraits.h"
#include "eSelectTimer.h"

int eSelectScreenManager::m_bPlayer1HasFinishedWaiting;
int eSelectScreenManager::m_bPlayer2HasFinishedWaiting;


int eSelectScreenManager::m_tSelectTickCounter;
int eSelectScreenManager::m_tSelectTickCounterP2;

int eSelectScreenManager::m_pSelectScreenProcessPointer;
int	 eSelectScreenManager::m_nSelectScreenCurrentCharacterDraw;

int eSelectScreenManager::m_pSelectScreenStringPointer;
std::string eSelectScreenManager::m_pSelectScreenLastString;

bool eSelectScreenManager::m_bCachedSoundData;
bool eSelectScreenManager::m_bDrawExtraCharacterP1;
eGameFlowData* eSelectScreenManager::m_pGameFlowData;

eMugenCharacterInfo* eSelectScreenManager::m_pCharacter;
MugenExplod eSelectScreenManager::m_expStageSelect = {};

void eSelectScreenManager::Init()
{
	m_bPlayer1HasFinishedWaiting = 0;
	m_bPlayer2HasFinishedWaiting = 0;
	m_pSelectScreenProcessPointer = 0;
	m_pSelectScreenStringPointer = 0;
	m_pSelectScreenLastString = "";
	m_pCharacter = nullptr;
	m_tSelectTickCounter = 0;
	m_tSelectTickCounterP2 = 0;
	m_bCachedSoundData = false;
	m_nSelectScreenCurrentCharacterDraw = 0;
	m_bDrawExtraCharacterP1 = true;
	eLog::PushMessage(__FUNCTION__, "Initialize\n");

	if (eSettingsManager::bEnableAfterSelectionPause)
		InjectHook(0x406D19, HookWaitAtCharacterSelect, PATCH_JUMP);

	Patch<int>(0x408A5F + 6, (int)HookSelectScreenProcess);
	InjectHook(0x407453, ProcessDrawingCharacterFace, PATCH_CALL);
	InjectHook(0x404029, HookSelectIDs, PATCH_JUMP);
	InjectHook(0x40380D, HookLoadCharacterData, PATCH_CALL);

	Nop(0x408AA3, 7);
	InjectHook(0x408AA3, HookCurrentScreenGameData, PATCH_JUMP);

	if (eSettingsManager::bDisplayStageSelectionOnlyWhenCharactersSelected)
	{
		Nop(0x407E15, 7);
		InjectHook(0x407E15, HookStageDisplay, PATCH_JUMP);
	}
	InjectHook(0x40EA37, Hook_DrawSprites, PATCH_JUMP);
	//InjectHook(0x409724, Hook_ReadSpriteConfig, PATCH_JUMP);
}

void eSelectScreenManager::Process()
{
	if (eSettingsManager::bEnableAfterSelectionPause)
	{
		if (eCursor::Player1_Character > -1 || eCursor::Player2_Character > -1)
		{
			ProcessWaitPlayer1();
			ProcessWaitPlayer2();
		}
	}
	if (eSettingsManager::bDev_DisplayPos)
	{
		eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
		if (eCursor::Player1_Character > 0)
			printf("Player 1: Row: %d   Column: %d  Character ID: %d [%d] \r", eCursor::Player1_Row, eCursor::Player1_Column, CharactersArray[eCursor::Player1_Character].ID, eCursor::Player1_Character);
	}

	if (GetAsyncKeyState(VK_F5) && eSettingsManager::bDumpCharacterInfo) PrintCharacterData();

	int mode = eSystem::GetGameplayMode();
	if (!(mode == MODE_VERSUS || mode == MODE_TEAM_VERSUS || mode == MODE_TEAM_COOP || mode == MODE_TRAINING))
	{
		ProcessPlayer2JoinIn();
	}
	
	if (eSettingsManager::bRandomStageConfirmSounds && !eSettingsManager::bHookStageAnnouncer)
	{
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C) = eSettingsManager::iRandomStageGroup;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C + 4) = rand() % eSettingsManager::iRandomStageRandomMax;
	}
	if (eMenuManager::m_pSelectScreenDataPointer)
	{
		if (!eMenuManager::m_bWasCursorAdjusted)
		{
			// p1
			*(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4 + 12) = eSettingsManager::iDefaultGameModeCursor;
			// p2
			*(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4  + 180 + 12) = eSettingsManager::iDefaultGameModeCursor;
			eMenuManager::m_bWasCursorAdjusted = true;
		}

		if (eSettingsManager::bHookStageAnnouncer)
		{
				int stageSelection = *(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3908);
				eStageAnnouncerEntry sound = eStageAnnouncer::m_vStageEntries[eStageAnnouncer::FindStageData(stageSelection)];
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C) = sound.Group;
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C + 4) = sound.Index;
		}
	}

}

void eSelectScreenManager::ProcessEnd()
{
	eSelectTimer::Process();
	DrawJoinIn();
}

void __declspec(naked) eSelectScreenManager::HookWaitAtCharacterSelect()
{

	static int wait_continue = 0x406D26;
	static int wait_pause = 0x406D67;

	// auto set to true if no p2
	if (!(eCursor::Player2_Character > -1))
		m_bPlayer2HasFinishedWaiting = 1;

	if (m_bPlayer1HasFinishedWaiting == 1 && m_bPlayer2HasFinishedWaiting == 1)
	{
		_asm {
			mov[eax], ebx
			mov[eax + 4], edi
			jmp wait_continue
		}
	}
	else
	{
		_asm jmp wait_pause
	}

}

void eSelectScreenManager::ProcessWaitPlayer1()
{
	if (eCursor::Player1_Selected)
	{
		if (eSystem::GetTimer() - m_tSelectTickCounter <= eSettingsManager::iAfterSelectionPauseTime)  return;
		m_bPlayer1HasFinishedWaiting = 1;
		m_tSelectTickCounter = eSystem::GetTimer();
	}
	else
	{
		m_tSelectTickCounter = eSystem::GetTimer();
		m_bPlayer1HasFinishedWaiting = 0;
	}
}

void eSelectScreenManager::ProcessWaitPlayer2()
{
	if (eCursor::Player2_Selected)
	{
		if (eSystem::GetTimer() - m_tSelectTickCounterP2 <= eSettingsManager::iAfterSelectionPauseTime)  return;
		m_bPlayer2HasFinishedWaiting = 1;
		m_tSelectTickCounterP2 = eSystem::GetTimer();
	}
	else
	{
		m_tSelectTickCounterP2 = eSystem::GetTimer();
		m_bPlayer2HasFinishedWaiting = 0;
	}
}

void eSelectScreenManager::ProcessGameLoopEvent()
{
}

void eSelectScreenManager::HookSelectScreenProcess(int a1, int a2)
{
	m_pSelectScreenProcessPointer = a2;
	Call<0x406E10, int, int>(a1, a2);
}

void eSelectScreenManager::PrintCharacterData()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
	std::ofstream oFile("characters.txt", std::ofstream::binary);

	if (CharactersArray)
	{
		oFile << "ID \t Flags \t File" << std::endl;
		for (int i = 0; i < eSystem::GetCharactersAmount(); i++)
		{

			oFile << "" << CharactersArray[i].ID << "\t ";
			if (CharactersArray[i].CharacterFlag & CHAR_FLAG_HIDDEN)
				oFile << "H";
			if (CharactersArray[i].CharacterFlag & CHAR_FLAG_VARIATIONS)
				oFile << "V";
			if (CharactersArray[i].CharacterFlag & CHAR_FLAG_BONUS)
				oFile << "B";
			if (CharactersArray[i].CharacterFlag & CHAR_FLAG_ANIM_ICON)
				oFile << "AI";
			
			std::string chrName = CharactersArray[i].FileName;
			std::string chrFolderName = CharactersArray[i].FolderName;
			std::string chr = chrFolderName + chrName;
			oFile << "\t " << chr.c_str();

			oFile << std::endl;
		}
		oFile.close();
	}
}
int data;
int eSelectScreenManager::ProcessDrawingCharacterFace(int * a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
	eMugenCharacterInfo* character;
	_asm mov character, ecx


	if (eSettingsManager::bHookAnimatedIcons)
	{
		if (character->CharacterFlag & CHAR_FLAG_ANIM_ICON)
		{
			eAnimatedIcons::Animate(character);
		}
		else
		{
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x230 + 4) = eSystem::iPortraitGroup;
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x230 + 8) = eSystem::iPortraitIndex;
		}

	}

	if (!(character->CharacterFlag & CHAR_FLAG_HIDDEN))
		return ((int(__cdecl*)(int*, int, int, int, int, int, int))0x411C00)(a1, a2, a3, a4, a5, a6, a7);
	else
		return -1;
}

int eSelectScreenManager::HookStageNumeration(char * dest, char * format, int dummy, char * name)
{
	return sprintf(dest, format, name);
}

void __declspec(naked) eSelectScreenManager::HookSelectIDs()
{
	static int select_done_jmp = 0x404060;
	static int select_continue_jmp = 0x404F6D;

	_asm {
		lea eax, [esp + 320]
		mov m_pSelectScreenStringPointer, eax
		mov eax, [esp + 20]
		mov m_pCharacter, eax
		pushad
	}
	m_pSelectScreenLastString = (char*)m_pSelectScreenStringPointer;


	if (m_pSelectScreenLastString == "randomselect")
	{
		m_pCharacter->ID = -2;
		m_pCharacter->SpritePointer = 0;

		_asm {
			popad
			jmp select_continue_jmp
		}
	}
	else if (m_pSelectScreenLastString == "magicbox")
	{
		m_pCharacter->ID = -3;
		m_pCharacter->SpritePointer = 0;
		sprintf(m_pCharacter->Name, "Magic Box");
		_asm {
			popad
			jmp select_continue_jmp
		}
	}
	else
	{
		_asm {
			popad
			jmp select_done_jmp
		}
	}

}

void __declspec(naked) eSelectScreenManager::HookCurrentScreenGameData()
{
	static int gamedata_done_jmp = 0x408AAA;
	_asm {
		mov eax, esi
		mov m_pGameFlowData, eax
		mov esi, 0x40E7C0
		call esi
		mov esi, m_pGameFlowData
		pushad
		popad
		jmp gamedata_done_jmp
	}
}


void eSelectScreenManager::ProcessPlayer2JoinIn()
{
	if (!eSystem::pushstart_set.active)
		return;
	if (eCursor::Player1_Selected || eCursor::Player2_Selected)
		return;
	if (!eMenuManager::m_pSelectScreenDataPointer)
		return;

	bool joined = false;



	if (eCursor::Player2_Character > -1)
	{
		if (eInputManager::CheckLastPlayer() == false)
		{
			if (eInputManager::GetKeyAction(INPUT_ACTION_START))
			{
				if (!eCursor::Player2_Selected && eCursor::Player2_Turns < 1)
					joined = true;
			}
		}
	}
	else
	{
		if (eCursor::Player1_Character > -1)
		{
			if (eInputManager::CheckLastPlayer() == true)
			{
				if (eInputManager::GetKeyAction(INPUT_ACTION_START))
				{
					if (!eCursor::Player1_Selected && eCursor::Player1_Turns < 1)
						joined = true;
				}
			}
		}

	}
	
	eGameplayModes mode = MODE_VERSUS;
	eGameplayModes curMode = (eGameplayModes)eSystem::GetGameplayMode();

	switch (curMode)
	{
	case MODE_ARCADE:
		mode = MODE_VERSUS;
		break;
	case MODE_TEAM_ARCADE:
		mode = MODE_TEAM_VERSUS;
		break;
	default:
		mode = MODE_VERSUS;
		break;
	}

	if (joined)
	{
		// when refreshing screen cursor gets set to starting pos
		// so current cursor is saved as setting then reset at select
		// screen init
		eCursor::StoreCursor();
		eSystem::ClearScreenparams(m_pGameFlowData->params);
		eSystem::SetGameplayMode(mode);
		eSystem::SetScreenParams(m_pGameFlowData->params, mode, 1);
		
		// refresh timer on join in
		eSelectTimer::Init();

		if (eSystem::pushstart_set.group >= 0 && eSystem::pushstart_set.index >= 0)
		{
			Sound* snd = eSystem::GetSystemSND();
			PlaySound(snd, eSystem::pushstart_set.group, eSystem::pushstart_set.index, 0, 3.390625f);
		}

		if (mode == MODE_VERSUS)
		{
			*(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4) = 0;
			*(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4 + 180) = 0;
		}
	}

}

void eSelectScreenManager::ProcessTestDraw()
{
	//return;

	
}

void eSelectScreenManager::ProcessScreenTimer()
{
}

void eSelectScreenManager::HookLoadCharacterData(char * file)
{
	for (int i = 0; i < eSystem::GetCharactersAmount(); i++)
	{
		eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
		eSoundEntry snd;
		snd.IsCached = false;
		snd.SoundData = nullptr;
		eCustomCursorManager::SoundCellTable.push_back(snd);
	}


	if (eSettingsManager::iCursorTableOperationType == MODE_CHAR_FILE)
	{
		if (eSettingsManager::bUseThreadForLoading)
			CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(THREAD_CacheSoundData), nullptr, 0, nullptr);
		else
			HookCacheSoundData();

	}


}

void eSelectScreenManager::THREAD_CacheSoundData()
{
	while (!m_bCachedSoundData)
		HookCacheSoundData();
}

void __declspec(naked) eSelectScreenManager::HookStageDisplay()
{
	static int jmp_continue = 0x407E1C;
	static int jmp_false = 0x407EF5;

	if (!(eCursor::Player1_Selected && eCursor::Player2_Selected))
	{
		_asm {
			jmp jmp_false
		}

	}
	else if (eCursor::Player2_Character < 0 && !eCursor::Player1_Selected)
	{
		_asm {
			jmp jmp_false
		}
	}
	else if (eCursor::Player1_Character < 0 && !eCursor::Player2_Selected)
	{
		_asm {
			jmp jmp_false
		}
	}
	else
	{
		_asm {
			cmp     dword ptr[edx + 15264], 0
			jmp jmp_continue
		}
	}

}

void eSelectScreenManager::DrawJoinIn()
{
	if (!eSystem::pushstart_set.active)
		return;

	static int font = 0;

	if (!font)
		font = LoadFont(eSystem::pushstart_set.font, 1);


	if (eSystem::GetGameFlow() == FLOW_SELECT_SCREEN)
	{
		if (eSystem::GetGameplayMode() == MODE_ARCADE || eSystem::GetGameplayMode() == MODE_TEAM_ARCADE)
		{
			int gameModeView = *(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4);
			int gameModeView_p2 = *(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4 + 180);

			eTextParams params = {};
			params.m_letterSpacingX = 1.0;
			params.m_letterSpacingY = 1.0;
			params.field_50 = 1.0;
			params.m_skew = 1.0;
			params.field_3C = 0.0;
			params.rotateY = 1.0;
			params.field_4 = 0.0;

			params.field_5C = 0xFFFF;
			params.m_hasBackground = 1;

			params.m_scaleX = eSystem::pushstart_set.scale_x;
			params.m_scaleY = eSystem::pushstart_set.scale_y;
			params.m_xPos = eSystem::pushstart_set.p1_x;
			params.m_yPos = eSystem::pushstart_set.p1_y;

			eTextParams params2 = params;
			params2.m_xPos = eSystem::pushstart_set.p2_x;
			params2.m_yPos = eSystem::pushstart_set.p2_y;

			if (eCursor::Player1_Character == -1 && !eCursor::Player2_Selected)
			{
				if (eCursor::Player2_Turns < 1)
				{
					if (gameModeView == 0)
					{
						if (eSystem::pushstart_set.flash == PUSHSTART_FLASH_BLINK)
						{
							if (eSystem::GetTimer() & eSystem::pushstart_set.flash_rate)
								Draw2DText(eSystem::pushstart_set.text, font, &params, eSystem::pushstart_set.color_r, eSystem::pushstart_set.color_g, eSystem::pushstart_set.color_b, eSystem::pushstart_set.p2_align);
						}
						else if (eSystem::pushstart_set.flash == PUSHSTART_FLASH_COLOR)
						{
							if (eSystem::GetTimer() & eSystem::pushstart_set.flash_rate)
								Draw2DText(eSystem::pushstart_set.text, font, &params, eSystem::pushstart_set.color_r, eSystem::pushstart_set.color_g, eSystem::pushstart_set.color_b, eSystem::pushstart_set.p2_align);
							else
								Draw2DText(eSystem::pushstart_set.text, font, &params, eSystem::pushstart_set.color2_r, eSystem::pushstart_set.color2_g, eSystem::pushstart_set.color2_b, eSystem::pushstart_set.p2_align);
						}
						else
							Draw2DText(eSystem::pushstart_set.text, font, &params, eSystem::pushstart_set.color_r, eSystem::pushstart_set.color_g, eSystem::pushstart_set.color_b, eSystem::pushstart_set.p2_align);
					}
				}
			}

			if (eCursor::Player2_Character == -1 && !eCursor::Player1_Selected)
			{
				if (eCursor::Player1_Turns < 1)
				{
					if (gameModeView_p2 == 0)
					{
						if (eSystem::pushstart_set.flash == PUSHSTART_FLASH_BLINK)
						{
							if (eSystem::GetTimer() & eSystem::pushstart_set.flash_rate)
								Draw2DText(eSystem::pushstart_set.text, font, &params2, eSystem::pushstart_set.color_r, eSystem::pushstart_set.color_g, eSystem::pushstart_set.color_b, eSystem::pushstart_set.p1_align);
						}
						else if (eSystem::pushstart_set.flash == PUSHSTART_FLASH_COLOR)
						{
							if (eSystem::GetTimer() & eSystem::pushstart_set.flash_rate)
								Draw2DText(eSystem::pushstart_set.text, font, &params2, eSystem::pushstart_set.color_r, eSystem::pushstart_set.color_g, eSystem::pushstart_set.color_b, eSystem::pushstart_set.p1_align);
							else
								Draw2DText(eSystem::pushstart_set.text, font, &params2, eSystem::pushstart_set.color2_r, eSystem::pushstart_set.color2_g, eSystem::pushstart_set.color2_b, eSystem::pushstart_set.p1_align);
						}
						else
							Draw2DText(eSystem::pushstart_set.text, font, &params2, eSystem::pushstart_set.color_r, eSystem::pushstart_set.color_g, eSystem::pushstart_set.color_b, eSystem::pushstart_set.p1_align);
					}
				}

			}
		}
	}
}

void __declspec(naked) eSelectScreenManager::Hook_ReadSpriteConfig()
{
	static int  jmp_point = 0x40972A;
	static int _edi = 0;
	_asm mov _edi, edi
	readExplodConfig("stageSelect", _edi, &m_expStageSelect);
	_asm {
		mov esi,ds:0x503388
		jmp jmp_point
	}
}

void eSelectScreenManager::HideSelectScreenSprites(bool characterOnly)
{
	if (!characterOnly)
	{
		Patch<char>(0x406F41 + 1, 0x84);
		Patch<char>(0x407125 + 1, 0x8E);

		char cmpBytes[5] = { 0x83, 0x7C, 0x84, 0x38, 0x00 };
		for (int i = 0; i < 5; i++)
		{
			Patch<char>(0x407826 + i, cmpBytes[i]);
		}
	}
	else
	{
		Patch<char>(0x4075A1 + 1, 0x85);
		Patch<char>(0x4075A1 + 1, 0x84);
		Nop(0x407826, 5);
		Patch<char>(0x406F41 + 1, 0x85);
		Patch<char>(0x407125 + 1, 0x85);

	}
}

void __declspec(naked) eSelectScreenManager::Hook_DrawSprites()
{
	static int jmp_cursor = 0x40EA3E;
	static int cur_func = 0;
	static int target_func = (int)&eSelectScreenManager::HookSelectScreenProcess;
	static int original_eax = 0;
	_asm {
		mov     edx, [ebx + 4]
		push    edx
		push    ebx
		mov cur_func, eax
		mov original_eax, eax
		pushad
	}
	if (cur_func == target_func && eSystem::face_draw_priority == FACEDRAW_ONTOP)
	{
		HideSelectScreenSprites(false);
		_asm {
			popad
			call eax
			pushad
		}
		HideSelectScreenSprites(true);
#ifdef MULTICHAR_DEBUG
		if (eAnimatedPortraits::iCurrentPlayerDrawID == 0)
		{
			_asm 		popad

			m_bDrawExtraCharacterP1 = true;
			{
				for (m_nSelectScreenCurrentCharacterDraw = 0; m_nSelectScreenCurrentCharacterDraw < eCursor::GetPlayerSelections(0); m_nSelectScreenCurrentCharacterDraw++)
				{
					_asm {
						mov    eax, original_eax
						call eax
					}
				}
			}

			m_bDrawExtraCharacterP1 = false;
			_asm {
				mov    eax, original_eax
				call eax
			}
		}
#else
		_asm {
			popad
			mov    eax, original_eax
			call eax
		}
#endif // MULTICHAR_DEBUG
	}
	else
	{
		_asm {
			popad
			call eax
		}
	}

	_asm jmp jmp_cursor
}

void eSelectScreenManager::HookCacheSoundData()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
	std::vector<eMugenCharacterInfo> CharactersArray_copy;

	// copy so variation changes don't mess up
	for (int i = 0; i < eSystem::GetCharactersAmount(); i++)
	{
		CharactersArray_copy.push_back(CharactersArray[i]);
	}
	for (int i = 0; i < eSystem::GetCharactersAmount(); i++)
	{
		eCustomCursorManager::SoundCellTable[i].CharID = CharactersArray_copy[i].ID;
	}


	for (unsigned int i = 0; i < eCustomCursorManager::SoundCellTable.size(); i++)
	{


		std::string ini_path = CharactersArray_copy[i].FolderName;
		ini_path += CharactersArray_copy[i].FileName;;

		CIniReader ini((char*)ini_path.c_str());

		std::string path = CharactersArray_copy[i].FolderName;

		eLog::PushMessage(__FUNCTION__, "Processing %d/%d\n", i + 1, eSystem::GetCharactersAmount());

		if (path.length() > 0)
		{
			path += ini.ReadString("Files", "sound", 0);

			std::size_t found = path.find_first_of(".");
			path = path.substr(0, found + strlen("snd") + 1);
			
			Sound* sound = LoadSNDFile(path.c_str());

			if (sound)
			{
				eLog::PushMessage(__FUNCTION__, "Loaded sound data for %s [%d] (%s)\n", CharactersArray_copy[i].FileName, CharactersArray_copy[i].ID, path.c_str());
				eCustomCursorManager::SoundCellTable[i].CharID = CharactersArray_copy[i].ID;
				eCustomCursorManager::SoundCellTable[i].SoundData = sound;
				eCustomCursorManager::SoundCellTable[i].IsCached = true;

			}
			else
			{
				eLog::PushMessage(__FUNCTION__, "Failed to load sound data for %s!\n", CharactersArray_copy[i].FileName);
				eLog::PushError();
			}
		}
		m_bCachedSoundData = true;
	}
	eLog::PushMessage(__FUNCTION__, "Character sounds cached!\n");
	CharactersArray_copy.clear();
}
