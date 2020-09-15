#pragma once

class eTagFix {
public:
	static int m_pParam;
	static int m_nFighters;

	static void Init();
	static void  Hook();
	static void  HookGameModeCommand();
};