#pragma once
#include <vector>
#include <string>
#include "../mugen/Data.h"


struct eTextDrawEntry {
	bool m_bIsActive;
	int  m_pFont;
	char m_szDisplayText[512] = {};
};



enum eNewCNSCommands {
	SetRoundTime = 2540,
	AddRoundTime,
	SetBGM,
	SetStage
};

union eMugenVar {
	int number;
	char* string;
	float real;
};

struct eMugenMachine {
	char pad[16];
	int commandID;
	char _pad[4];
	int* var;
};


class eScriptProcessor {
public:

	static eMugenMachine* vm;
	static int			  vm_cur_line;
	static int			  vm_buff;
	static int			  vm_cur_proc;
	static bool			  updateMusic;
	static std::vector<std::string> stringTable;

	static void Init();
	static void Hook2DDraw();
	static void ProcessCustomDraws();
	static int  GetScreenResX();

	static int GetCommandID(int esp);
	static void GetCommandID_Hook();

	static void ExecuteCommand_Hook();
	static void ExecuteCommand(int id);

	static bool IsCommandValid(int id);

	static void AddStringToTable(std::string str);
	static int  FindString(std::string str);


	// parsers
	static void PARSE_SetBGM();
	static void PARSE_SetStage();
	static void PARSE_AddRoundTime();
	static void PARSE_SetRoundTime();

	// commands
	static void CMD_SetBGM();
	static void CMD_AddRoundTime();
	static void CMD_SetRoundTime();
	static void CMD_SetStage();


	// functions

	static void SwapStage(char* name);
};
