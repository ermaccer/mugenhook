#pragma once
#include "..\mugen\System.h"
#include "..\mugen\Sprite.h"
#include <string>

#define CHAR_FLAG_HIDDEN 1
#define CHAR_FLAG_VARIATIONS 2
#define CHAR_FLAG_BONUS 4
#define CHAR_FLAG_ANIM_ICON 8




class eSelectScreenManager {
public:

	static MugenExplod m_expStageSelect;

	static bool m_bCachedSoundData;
	static bool m_bDrawExtraCharacterP1;

	static int  m_bPlayer1HasFinishedWaiting;
	static int  m_bPlayer2HasFinishedWaiting;
	static int  m_tSelectTickCounter;
	static int  m_tSelectTickCounterP2;
	static int  m_pSelectScreenProcessPointer;
	static int  m_pSelectScreenStringPointer;

	static int	m_nSelectScreenCurrentCharacterDraw;

	static eGameFlowData* m_pGameFlowData;

	static std::string  m_pSelectScreenLastString;
	static eMugenCharacterInfo* m_pCharacter;
	static void Init();
	static void Process();
	static void ProcessEnd();
	static void HookWaitAtCharacterSelect();
	static void ProcessWaitPlayer1();
	static void ProcessWaitPlayer2();
	static void ProcessGameLoopEvent();
	static void HookSelectScreenProcess(int a1, int a2);
	static void PrintCharacterData();
	static int  ProcessDrawingCharacterFace(int* a1, int a2, int a3, int a4, int a5, int a6, int a7);
	static int  HookStageNumeration(char* dest, char* format, int dummy, char* name);
	static void HookSelectIDs();

	static void HookCurrentScreenGameData();

	static void ProcessPlayer2JoinIn();
	static void ProcessTestDraw();
	static void ProcessScreenTimer();

	static void HookLoadCharacterData(char* file);
	static void HookCacheSoundData();
	static void THREAD_CacheSoundData();

	static void HookStageDisplay();

	static void DrawJoinIn();

	static void Hook_ReadSpriteConfig();


	// drawing hacks
	static void HideSelectScreenSprites(bool characterOnly);

	static void Hook_DrawSprites();
};