#include "eSelectScreenManager.h"
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


int eSelectScreenManager::m_bPlayer1HasFinishedWaiting;
int eSelectScreenManager::m_bPlayer2HasFinishedWaiting;


int eSelectScreenManager::m_tSelectTickCounter;
int eSelectScreenManager::m_tSelectTickCounterP2;

int eSelectScreenManager::m_pSelectScreenProcessPointer;


int eSelectScreenManager::m_pSelectScreenStringPointer;
std::string eSelectScreenManager::m_pSelectScreenLastString;

eMugenCharacterInfo* eSelectScreenManager::m_pCharacter;

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

	eLog::PushMessage(__FUNCTION__, "Initialize\n");

	if (eSettingsManager::bEnableAfterSelectionPause)
		InjectHook(0x406D19, HookWaitAtCharacterSelect, PATCH_JUMP);

	Patch<int>(0x408A5F + 6, (int)HookSelectScreenProcess);
	InjectHook(0x407453, ProcessDrawingCharacterFace, PATCH_CALL);
	InjectHook(0x404029, HookSelectIDs, PATCH_JUMP);
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


	if (eSettingsManager::bDev_DisplayPos) printf("Player 1: Row: %d   Column: %d  Character: %d \r", eCursor::Player1_Row, eCursor::Player1_Column, eCursor::Player1_RandomCharacter);

	if (GetAsyncKeyState(VK_F5) && eSettingsManager::bDumpCharacterInfo) PrintCharacterData();


	if (eSettingsManager::bRandomStageConfirmSounds && !eSettingsManager::bHookStageAnnouncer)
	{
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C) = eSettingsManager::iRandomStageGroup;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C + 4) = rand() % eSettingsManager::iRandomStageRandomMax;
	}

	if (eSettingsManager::bHookStageAnnouncer)
	{
		if (eMenuManager::m_pSelectScreenDataPointer)
		{
			int stageSelection = *(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3908);
			eStageAnnouncerEntry sound = eStageAnnouncer::m_vStageEntries[eStageAnnouncer::FindStageData(stageSelection)];

			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C) = sound.Group;
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x36C + 4) = sound.Index;
		}

	}

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
		for (int i = 0; i < eSystem::iRows * eSystem::iColumns; i++)
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
