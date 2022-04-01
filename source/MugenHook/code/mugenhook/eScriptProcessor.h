#pragma once

struct eTextDrawEntry {
	bool m_bIsActive;
	int  m_pFont;
	char m_szDisplayText[512] = {};
};



enum eNewCNSCommands {
	SetRoundTime = 2540,
};

struct eMugenMachine{
	char pad[16];
	int commandID;
};

class eScriptProcessor {
public:

	static eMugenMachine* vm;

	static void Init();
	static void Hook2DDraw();
	static void ProcessCustomDraws();
	static int  GetScreenResX();

	static int GetCommandID(int esp);
	static void GetCommandID_Hook();

	static void ExecuteCommand_Hook();
	static void ExecuteCommand(int id);

	static bool IsCommandValid(int id);
};