#pragma once
#include <vector>
#include <string>
#include "../mugen/Data.h"


enum eNewCNSCommands {
	SetRoundTime = 2540,
	AddRoundTime,
	SetBGM,
	SetStage,
	TextDraw,
	DisableTextDraw,
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


enum TextDraw_Params {
	TDP_Align = 12 * 2,
	TDP_PosX = 12 * 3,
	TDP_PosY = 12 * 4,
	TDP_ScaleX = 12 * 5,
	TDP_ScaleY = 12 * 6,
	TDP_R = 12 * 7,
	TDP_G = 12 * 8,
	TDP_B = 12 * 9,
	TDP_ID = 12 * 10,
	TDP_STRING = 12 * 11,
	TDP_Order = 12 * 12,
	TDP_Duration = 12 * 13,
	TDP_Font = 12 * 14,
	TDP_Flags = 12 * 15,
	TDP_ArgsNum = 12 * 16,
	TDP_ArgTypes = 12 * 17,
	TDP_Arg0 = 12 * 18,
	TDP_Arg1 = 12 * 19,
	TDP_Arg2 = 12 * 20,
};



class eScriptProcessor {
public:

	static eMugenMachine* vm;
	static int			  vm_cur_line;
	static int			  vm_buff;
	static int			  vm_cur_proc;
	static int			  vm_last_obj;
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
	static void CMD_TextDraw();
	static void CMD_DisableTextDraw();

	// helpers
	static int GetCharacterIDFromObject(eMugenCharacter* obj);


	// functions

	static void SwapStage(char* name);
};
