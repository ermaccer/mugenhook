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

eMugenMachine* eScriptProcessor::vm;
int eScriptProcessor::vm_cur_line;
int eScriptProcessor::vm_buff;
int eScriptProcessor::vm_cur_proc;
bool			  eScriptProcessor::updateMusic = false;
std::vector<std::string> eScriptProcessor::stringTable;


void eScriptProcessor::Init()
{
	InjectHook(0x40DF2D, eScriptProcessor::Hook2DDraw, PATCH_JUMP);
	InjectHook(0x444F14, eScriptProcessor::GetCommandID_Hook, PATCH_JUMP);
	InjectHook(0x44904E, eScriptProcessor::ExecuteCommand_Hook, PATCH_JUMP);
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
