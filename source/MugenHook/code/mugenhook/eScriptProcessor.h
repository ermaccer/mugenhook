#pragma once

struct eTextDrawEntry {
	bool m_bIsActive;
	int  m_pFont;
	char m_szDisplayText[512] = {};
};

class eScriptProcessor {
public:
	static void Init();
	static void Hook2DDraw();
	static void ProcessCustomDraws();
	static int  GetScreenResX();
};