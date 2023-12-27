#include "eScriptProcessor.h"
#include "base.h"
#include "..\core\eInputManager.h"
#include "..\mugen\System.h"
#include "..\mugen\Draw.h"
#include "..\core\eCursor.h"
#include "..\mugen\System.h"
#include "..\mugen\Sound.h"
#include "eSelectScreenManager.h"
#include "..\core\eLog.h"
#include <algorithm>
#include <filesystem>
#include "eTextDraw.h"

eMugenMachine* eScriptProcessor::vm;
int eScriptProcessor::vm_cur_line;
int eScriptProcessor::vm_buff;
int eScriptProcessor::vm_cur_proc;
int			  eScriptProcessor::vm_last_obj;
bool			  eScriptProcessor::updateMusic = false;
std::vector<std::string> eScriptProcessor::stringTable;


void eScriptProcessor::Init()
{
	InjectHook(0x40DF2D, eScriptProcessor::Hook2DDraw, PATCH_JUMP);
	InjectHook(0x444F14, eScriptProcessor::GetCommandID_Hook, PATCH_JUMP);
	InjectHook(0x44904E, eScriptProcessor::ExecuteCommand_Hook, PATCH_JUMP);
	eTextDrawProcessor::InitTextDraw();
}

void __declspec(naked) eScriptProcessor::Hook2DDraw()
{
	static int draw_continue = 0x40DF32;
	ProcessCustomDraws();

	_asm {
		mov eax, ds: 0x5040E8
		pushad
		popad
		jmp  draw_continue
	}
}


void eScriptProcessor::ProcessCustomDraws()
{
	eTextDrawProcessor::UpdateTextDraws();
}

int eScriptProcessor::GetScreenResX()
{
	return *(int*)0x5050E8;
}

int eScriptProcessor::GetCommandID(int esp)
{
	int result = 0;
	char* commandName = (char*)(esp + 32);

	if (strcmp(commandName, "setroundtime") == 0)
	{ 
		vm->commandID = SetRoundTime;
		char* value = (char*)CNS_ReadValue(vm_cur_line, "value");
		if (value)
		{
			if (CNS_StoreValue(value, (int)vm + 24, vm_buff, 0, 1))
				result = 1;
		}
		else
		{
			eLog::PushMessage(__FUNCTION__, "Couldn't find 'value' for %d!", SetRoundTime);
			eLog::PushError();
			result = 0;
		}
	}
	else if (strcmp(commandName, "addroundtime") == 0)
	{
		vm->commandID = AddRoundTime;
		char* value = (char*)CNS_ReadValue(vm_cur_line, "value");
		if (value)
		{
			if (CNS_StoreValue(value, (int)vm + 24, vm_buff, 0, 1))
				result = 1;
		}
		else
		{
			eLog::PushMessage(__FUNCTION__, "Couldn't find 'value' for %d!", AddRoundTime);
			eLog::PushError();
			result = 0;
		}
		char* subtract = (char*)CNS_ReadValue(vm_cur_line, "subtract");
		if (subtract)
			CNS_StoreValue(subtract, (int)vm + 36, vm_buff, 0, 1);
		else
			eLog::PushMessage(__FUNCTION__, "No 'subtract' for %d!", AddRoundTime);
	}
	else if (strcmp(commandName, "setbgm") == 0)
	{
		vm->commandID = SetBGM;


		char* value = (char*)CNS_ReadValue(vm_cur_line, "value");
		if (value)
		{
			std::string str = value;
			str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
			if (str == "restore")
			{
				if (CNS_StoreValue("1", (int)vm + 36, vm_buff, 0, 1))
					result = 1;
			}
			else
			{
				AddStringToTable(str);
				int id = FindString(str);
				std::string output = std::to_string(id);
				if (CNS_StoreValue((char*)output.c_str(), (int)vm + 24, vm_buff, 0, 1))
					result = 1;
				if (CNS_StoreValue("0", (int)vm + 36, vm_buff, 0, 1))
					result = 1;
			}

		}

		char* restore = (char*)CNS_ReadValue(vm_cur_line, "restore");
		if (restore)
		{
			if (CNS_StoreValue(restore, (int)vm + 36, vm_buff, 0, 1))
				result = 1;
		}

		result = 1;
	}
	else if (strcmp(commandName, "setstage") == 0)
	{
		vm->commandID = SetStage;

		char* value = (char*)CNS_ReadValue(vm_cur_line, "value");
		if (value)
		{
			std::string str = value;
			str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
			AddStringToTable(str);
			int id = FindString(str);
			std::string output = std::to_string(id);
			if (CNS_StoreValue((char*)output.c_str(), (int)vm + 24, vm_buff, 0, 1))
				result = 1;

		}
		result = 1;
	}
	else if (strcmp(commandName, "textdraw") == 0)
	{
		vm->commandID = TextDraw;
		int newVM = mugen_calloc(256);
		if (newVM)
		{
			char* align = (char*)CNS_ReadValue(vm_cur_line, "align");
			if (align)
			{
				if (CNS_StoreValue(align, (int)newVM + TDP_Align, vm_buff, 0, 1))
					result = 1;
			}

			char* posx = (char*)CNS_ReadValue(vm_cur_line, "x");
			if (posx)
			{
				if (CNS_StoreValue(posx, (int)newVM + TDP_PosX, vm_buff, 0, 1))
					result = 1;
			}
			char* posy = (char*)CNS_ReadValue(vm_cur_line, "y");
			if (posy)
			{
				if (CNS_StoreValue(posy, (int)newVM + TDP_PosY, vm_buff, 0, 1))
					result = 1;
			}
			char* xscale = (char*)CNS_ReadValue(vm_cur_line, "xscale");
			if (xscale)
			{
				if (CNS_StoreValue(xscale, (int)newVM + TDP_ScaleX, vm_buff, 0, 2))
					result = 1;
			}
			char* yscale = (char*)CNS_ReadValue(vm_cur_line, "yscale");
			if (yscale)
			{
				if (CNS_StoreValue(yscale, (int)newVM + TDP_ScaleY, vm_buff, 0, 2))
					result = 1;
			}
			char* r = (char*)CNS_ReadValue(vm_cur_line, "red");
			if (r)
			{
				if (CNS_StoreValue(r, (int)newVM + TDP_R, vm_buff, 0, 1))
					result = 1;
			}
			char* g = (char*)CNS_ReadValue(vm_cur_line, "green");
			if (g)
			{
				if (CNS_StoreValue(g, (int)newVM + TDP_G, vm_buff, 0, 1))
					result = 1;
			}
			char* b = (char*)CNS_ReadValue(vm_cur_line, "blue");
			if (b)
			{
				if (CNS_StoreValue(b, (int)newVM + TDP_B, vm_buff, 0, 1))
					result = 1;
			}
			char* id = (char*)CNS_ReadValue(vm_cur_line, "id");
			if (id)
			{
				if (CNS_StoreValue(id, (int)newVM + TDP_ID, vm_buff, 0, 1))
					result = 1;
			}
			char* order = (char*)CNS_ReadValue(vm_cur_line, "layerno");
			if (order)
			{
				if (CNS_StoreValue(order, (int)newVM + TDP_Order, vm_buff, 0, 1))
					result = 1;
			}
			char* duration = (char*)CNS_ReadValue(vm_cur_line, "duration");
			if (duration)
			{
				if (CNS_StoreValue(duration, (int)newVM + TDP_Duration, vm_buff, 0, 1))
					result = 1;
			}
			char* font = (char*)CNS_ReadValue(vm_cur_line, "font");
			if (font)
			{
				if (CNS_StoreValue(font, (int)newVM + TDP_Font, vm_buff, 0, 1))
					result = 1;
			}
			char* flags = (char*)CNS_ReadValue(vm_cur_line, "flags");
			if (flags)
			{
				if (CNS_StoreValue(flags, (int)newVM + TDP_Flags, vm_buff, 0, 1))
					result = 1;
			}
			char* argsNum = (char*)CNS_ReadValue(vm_cur_line, "ArgsNum");
			if (argsNum)
			{
				if (CNS_StoreValue(argsNum, (int)newVM + TDP_ArgsNum, vm_buff, 0, 1))
					result = 1;

				int argsNumInt = 0;
				sscanf(argsNum, "%d", &argsNumInt);
				printf("Processing args %d\n", argsNumInt);

				if (argsNumInt > 0)
				{
					char* argTypes = (char*)CNS_ReadValue(vm_cur_line, "ArgTypes");
					if (!argTypes)
					{
						eLog::PushMessage(__FUNCTION__, "Couldn't find 'argTypes' for %d despite argsNum being non zero!", TextDraw);
						eLog::PushError();
						result = 0;
					}

					int argTypesNum = 0;

					sscanf(argTypes, "%d", &argTypesNum);

					if (argTypesNum >= 100 && argTypesNum <= 222)
					{
						int argTypesArray[3] = {};
						argTypesArray[0] = argTypesNum / 100;
						argTypesArray[1] = (argTypesNum % 100) / 10;
						argTypesArray[2] = (argTypesNum % 100) % 10;
						printf("ARG %d %d %d\n", argTypesArray[0], argTypesArray[1], argTypesArray[2]);
						if (argTypes)
						{
							if (CNS_StoreValue(argTypes, (int)newVM + TDP_ArgTypes, vm_buff, 0, 1))
								result = 1;

							char* arg0 = (char*)CNS_ReadValue(vm_cur_line, "Arg0");
							if (arg0)
							{
								if (argTypesArray[0] == ETextDrawArgType_Integer)
								{
									if (CNS_StoreValue(arg0, (int)newVM + TDP_Arg0, vm_buff, 0, 1))
										result = 1;
								}
								else
									if (CNS_StoreValue(arg0, (int)newVM + TDP_Arg0, vm_buff, 0, 2))
										result = 1;

							}
							char* arg1 = (char*)CNS_ReadValue(vm_cur_line, "Arg1");
							if (arg1)
							{
								if (argTypesArray[1] == ETextDrawArgType_Integer)
								{
									if (CNS_StoreValue(arg1, (int)newVM + TDP_Arg1, vm_buff, 0, 1))
										result = 1;
								}
								else
									if (CNS_StoreValue(arg1, (int)newVM + TDP_Arg1, vm_buff, 0, 2))
										result = 1;
							}
							char* arg2 = (char*)CNS_ReadValue(vm_cur_line, "Arg2");
							if (arg2)
							{
								if (argTypesArray[2] == ETextDrawArgType_Integer)
								{
									if (CNS_StoreValue(arg2, (int)newVM + TDP_Arg2, vm_buff, 0, 1))
										result = 1;
								}
								else
									if (CNS_StoreValue(arg2, (int)newVM + TDP_Arg2, vm_buff, 0, 2))
										result = 1;
							}
						}
					}
					else
					{
						eLog::PushMessage(__FUNCTION__, "Illegal 'argTypes' (%d) for %d!", argTypesNum, TextDraw);
						eLog::PushError();
						result = 0;
					}

				}

			}
			
			
			char* text = (char*)CNS_ReadValue(vm_cur_line, "text");
			if (text)
			{
				std::string str = text;
				str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
				for (unsigned int i = 0; i < str.length(); i++)
				{
					if (str[i] == 0x5C)
					{
						if (str[i + 1] == 0x6E)
						{
							str[i] = 0xD;
							str[i + 1] = 0xA;
						}
					}
				}
				AddStringToTable(str);

				int id = FindString(str);
				std::string output = std::to_string(id);
				if (CNS_StoreValue((char*)output.c_str(), (int)newVM + TDP_STRING, vm_buff, 0, 1))
					result = 1;
			
			}
			else
			{
				eLog::PushMessage(__FUNCTION__, "Couldn't find 'text' for %d!", TextDraw);
				eLog::PushError();
				result = 0;
			}
			result = 1;
			*(int*)((int)vm + 96) = (int)newVM;
		}
	}
	else if (strcmp(commandName, "disabletextdraw") == 0)
	{
		vm->commandID = DisableTextDraw;
		char* value = (char*)CNS_ReadValue(vm_cur_line, "id");
		if (value)
		{
			if (CNS_StoreValue(value, (int)vm + 24, vm_buff, 0, 1))
				result = 1;
		}
		else
		{
			eLog::PushMessage(__FUNCTION__, "Couldn't find 'id' for %d!", DisableTextDraw);
			eLog::PushError();
			result = 0;
		}
	}
	return result;
}

void __declspec(naked) eScriptProcessor::GetCommandID_Hook()
{
	static int cmd_continue = 0x444F1E;
	static int cmd_found = 0x4472C2;
	static int _esp = 0;

	_asm {
		mov		vm, ebx

		mov     dword ptr[esi], 0
		mov     byte ptr[esi + 4], 0
		mov _esp, esp
		mov vm_cur_line, ebp
		mov vm_buff, edi
		mov vm_last_obj, 0
		pushad
	}
	if (GetCommandID(_esp))
	{
		_asm {
			popad
			jmp cmd_found
		}
	}
	else
	{
		_asm {
			popad
			jmp cmd_continue
		}
	}

}

void __declspec(naked) eScriptProcessor::ExecuteCommand_Hook()
{
	static int cmd_ex_continue = 0x449055;
	static int cmd_id = 0;
	_asm {
		mov[esp + 36], eax
		mov vm_last_obj, ebp
		mov     eax, [edi + 16]
		mov     vm, edi
		mov cmd_id, eax
		mov vm_cur_proc, ebp
		pushad
	}

	if (IsCommandValid(cmd_id))
	{
		ExecuteCommand(cmd_id);
		_asm {
			popad
			xor eax, eax
			pop edi
			pop esi
			pop ebp
			pop ebx
			add esp, 4336
			retn
		}
	}

	_asm {
		popad
		jmp cmd_ex_continue
	}
}

void eScriptProcessor::ExecuteCommand(int id)
{
	switch (id)
	{
	case SetBGM:
		CMD_SetBGM();
		break;
	case SetRoundTime:
		CMD_SetRoundTime();
		break;
	case AddRoundTime:
		CMD_AddRoundTime();
		break;
	case SetStage:
		CMD_SetStage();
		break;
	case TextDraw:
		CMD_TextDraw();
		break;
	case DisableTextDraw:
		CMD_DisableTextDraw();
		break;
	default:
		break;
	}
}

bool eScriptProcessor::IsCommandValid(int id)
{
	return id >= SetRoundTime;
}

void eScriptProcessor::AddStringToTable(std::string str)
{
	stringTable.push_back(str);
	stringTable.erase(std::unique(stringTable.begin(), stringTable.end()), stringTable.end());
}

int eScriptProcessor::FindString(std::string str)
{
	int result = -1;
	for (unsigned int i = 0; i < stringTable.size(); i++)
	{
		if (stringTable[i] == str)
		{
			result = i;
			break;
		}
	}
	return result;
}

void eScriptProcessor::PARSE_SetBGM()
{
}

void eScriptProcessor::PARSE_SetStage()
{
}

void eScriptProcessor::PARSE_AddRoundTime()
{
}

void eScriptProcessor::PARSE_SetRoundTime()
{
}

void eScriptProcessor::CMD_AddRoundTime()
{
	if (eSystem::GetGameplayMode() == MODE_TRAINING)
		return;

	int value = CNS_RecallValue(vm_cur_proc, (int)vm + 24, 0);
	int subtract = CNS_RecallValue(vm_cur_proc, (int)vm + 36, 0);

	int new_time = eSystem::GetRoundTime();
	if (subtract)
		new_time -= 60 * value;
	else
		new_time += 60 * value;

	if (new_time < 0)
		new_time = 0;

	eSystem::SetRoundTime(new_time);
}

void eScriptProcessor::CMD_SetRoundTime()
{
	if (eSystem::GetGameplayMode() == MODE_TRAINING)
		return;

	int value = CNS_RecallValue(vm_cur_proc, (int)vm + 24, 0);
	int new_time = value * 60;


	if (new_time < 0)
		new_time = 0;

	eSystem::SetRoundTime(new_time);
}

void eScriptProcessor::CMD_SetStage()
{
	eMugenData* data = *(eMugenData**)eSystem::pMugenDataPointer;
	int value = CNS_RecallValue(vm_cur_proc, (int)vm + 24, 0);

	SwapStage((char*)stringTable[value].c_str());
}

void eScriptProcessor::CMD_TextDraw()
{
	eMugenData* data = eSystem::GetData();

	eMugenCharacter* cur_obj = (eMugenCharacter*)vm_last_obj;

	if (cur_obj)
	{

		eTextDrawArg args[TEXTDRAW_MAX_ARGS];
		static char ms_textDrawTempStr[512] = {};
		int buff = *(int*)((int)vm + 96);
		int align = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Align, 0);

		int x = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_PosX, 0);
		int y = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_PosY, 0);
		float xscale = CNS_RecallFloat(vm_cur_proc, (int)buff + TDP_ScaleX);
		float yscale = CNS_RecallFloat(vm_cur_proc, (int)buff + TDP_ScaleY);
		int r = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_R, 0);
		int g = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_G, 0);
		int b = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_B, 0);
		int tID = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_ID, 0);
		int strID = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_STRING, 0);
		int order = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Order, 0);
		int duration = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Duration, 0);
		int fontID = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Font, 0);
		int flags = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Flags, 0);
		int argsNum = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_ArgsNum, 0);
		int argTypes = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_ArgTypes, 0);

		int argTypesArray[3] = {};
		argTypesArray[0] = argTypes / 100;
		argTypesArray[1] = (argTypes % 100) / 10;
		argTypesArray[2] = (argTypes % 100) % 10;


		if (argTypesArray[0] == ETextDrawArgType_Integer)
			args[0].integerData = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Arg0, 0);
		else
			args[0].floatData = CNS_RecallFloat(vm_cur_proc, (int)buff + TDP_Arg0);

		if (argTypesArray[1] == ETextDrawArgType_Integer)
			args[1].integerData = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Arg1, 0);
		else
			args[1].floatData = CNS_RecallFloat(vm_cur_proc, (int)buff + TDP_Arg1);

		if (argTypesArray[2] == ETextDrawArgType_Integer)
			args[2].integerData = CNS_RecallValue(vm_cur_proc, (int)buff + TDP_Arg2, 0);
		else
			args[2].floatData = CNS_RecallFloat(vm_cur_proc, (int)buff + TDP_Arg2);




		int id = GetCharacterIDFromObject(cur_obj);

		if (!(tID >= 0 && tID < TEXTDRAW_NUM))
		{
			eLog::PushMessage(__FUNCTION__, "TextDraw: Invalid ID! Valid range is 0-7, provided %d!", tID);
			eLog::PushError();
		}

		if (!(id == -1))
		{
			eTextParams params = {};
			eTextDrawConfig config = {};
			params.m_letterSpacingX = 1.0;
			params.m_letterSpacingY = 1.0;
			params.field_50 = 1.0;
			params.m_skew = 1.0;
			params.field_3C = 0.0;
			params.rotateY = 1.0;
			params.field_4 = 0.0;

			params.field_5C = 0xFFFF;
			params.m_alpha = 1;

			if (flags & ETextDrawFlags_NoBackground)
				params.m_alpha = 0;

			params.m_scaleX = xscale * eSystem::GetSystemScale();
			params.m_scaleY = yscale * eSystem::GetSystemScale();
			params.m_xPos = x * eSystem::GetSystemScale();
			params.m_yPos = y * eSystem::GetSystemScale();

			config.m_nAlign = align;
			config.m_red = r;
			config.m_blue = b;
			config.m_green = g;
			config.m_nLayerno = order;
			if (duration < 0)
				duration = 99999999;
			config.m_nDuration = duration;

			config.m_nFont = fontID;

			// todo: rewrite this somehow
			switch (argsNum)
			{
			case 1:
				switch (argTypes)
				{
				case 200:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData);
					break;
				default:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData);
					break;
				}
				break;
			case 2:
				switch (argTypes)
				{
				case 120:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData, args[1].floatData);
					break;					  
				case 220:					  
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData, args[1].floatData);
					break;					  
				case 210:					  
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData, args[1].integerData);
					break;					  
				default:					  
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData, args[1].integerData);
					break;
				}
				break;
			case 3:
				switch (argTypes)
				{
				case 222:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData, args[1].floatData, args[2].floatData);
					break;
				case 221:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData, args[1].floatData, args[2].integerData);
					break;
				case 122:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData, args[1].floatData, args[2].floatData);
					break;
				case 211:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData, args[1].integerData, args[2].integerData);
					break;
				case 212:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].floatData, args[1].integerData, args[2].floatData);
					break;
				case 112:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData, args[1].integerData, args[2].floatData);
					break;
				case 121:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData, args[1].floatData, args[2].integerData);
					break;
				default:
					sprintf(ms_textDrawTempStr, (char*)stringTable[strID].c_str(), args[0].integerData, args[1].integerData, args[2].integerData);
					break;
				}
				break;
			default:
				sprintf(ms_textDrawTempStr,  (char*)stringTable[strID].c_str());
				break;
			}
			eTextDrawProcessor::SetTextDraw((int)cur_obj, id, tID, params, config, ms_textDrawTempStr);
		}

	}


}

void eScriptProcessor::CMD_DisableTextDraw()
{
	eMugenCharacter* cur_obj = (eMugenCharacter*)vm_last_obj;

	if (cur_obj)
	{

		int value = CNS_RecallValue(vm_cur_proc, (int)vm + 24, 0);

		int id = GetCharacterIDFromObject(cur_obj);

		if (value == -1 && !(id == -1))
		{
			for (int i = 0; i < TEXTDRAW_NUM; i++)
				eTextDrawProcessor::DisableTextDraw(id, i);
		}

		if (!(value >= 0 && value < TEXTDRAW_NUM))
		{
			eLog::PushMessage(__FUNCTION__, "DisableTextDraw: Invalid ID! Valid range is 0-7, provided %d!", value);
			eLog::PushError();
			exit(0);
		}


		if (!(id == -1))
		{
			eTextDrawProcessor::DisableTextDraw(id, value);
		}

	}
}

int eScriptProcessor::GetCharacterIDFromObject(eMugenCharacter* obj)
{
	eMugenData* data = eSystem::GetData();
	int amount_p1 = data->GetTeamCharactersAmount(TEAM_1);

	if (amount_p1 >= 0)
	{
		for (int i = 0; i < amount_p1; i++)
		{
			int id = 1 + (i * 2);
			if (eMugenCharacter* chr = data->GetCharacter(id))
			{
				if (chr == obj)
					return id;

			}
		}
	}

	int amount_p2 = data->GetTeamCharactersAmount(TEAM_2);
	if (amount_p2 >= 0)
	{
		for (int i = 0; i < amount_p2; i++)
		{
			int id = 2 + (i * 2);
			if (eMugenCharacter* chr = data->GetCharacter(id))
			{
				if (chr == obj)
					return id;
			}
		}
	}
	return -1;
}

void eScriptProcessor::SwapStage(char* name)
{
	eMugenData* data = *(eMugenData**)eSystem::pMugenDataPointer;

	// do not update same stage
	if (!(strcmp(data->LastStage, name) == 0))
	{
		sprintf(data->LastStage, name);
		LoadStage(&data->StageData);
		LoadBGM(data->StageData.BGMusic);
	}

}

void eScriptProcessor::CMD_SetBGM()
{
	eMugenData* data = *(eMugenData**)eSystem::pMugenDataPointer;
	int value = CNS_RecallValue(vm_cur_proc, (int)vm + 24, 0);
	int restore = CNS_RecallValue(vm_cur_proc, (int)vm + 36, 0);
	if (restore == 0)
		LoadBGM((char*)stringTable[value].c_str());
	else
	{
		CIniReader stage(data->LastStage);
		char* music = stage.ReadString("Music", "bgmusic", 0);
		if (music)
			LoadBGM((char*)music);
	}
}
