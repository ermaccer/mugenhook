#pragma once

class eMenuManager {
public:
	static bool m_bIsInMainMenu;
	static bool m_bIsInSelectScreen;
	static bool m_bAnimsRequireRefresh;
	static int  m_fntOptions;

	static void Init();

	// menu hooks
	static int HookMainMenu();
	static int HookSelectScreenMenu();
	static int HookGameLoop();


};