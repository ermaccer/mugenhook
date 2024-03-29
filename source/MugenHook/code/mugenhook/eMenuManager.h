#pragma once

class eMenuManager {
public:
	static bool m_bIsInMainMenu;
	static bool m_bIsInSelectScreen;
	static bool m_bAnimsRequireRefresh;
	static bool m_bWasCursorAdjusted;
	static int  m_fntOptions;
	static int  m_pSelectScreenDataPointer;

	static void Init();

	// menu hooks
	static int HookMainMenu();
	static int HookSelectScreenMenu();
	static int HookGameLoop();
	static int HookBeginMatch();
	static void HookSelectScreenProcess();

	static bool IsGameModeSelectOn();
	static bool AreCharactersSelected();
};