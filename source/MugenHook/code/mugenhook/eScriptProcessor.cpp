#include "eScriptProcessor.h"
#include "base.h"
#include "..\core\eInputManager.h"
#include "..\mugen\System.h"
#include "..\mugen\Draw.h"
#include "..\core\eCursor.h"
#include "..\mugen\System.h"
#include "eSelectScreenManager.h"

eMugenMachine* eScriptProcessor::vm;

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
		mov     dword ptr[esi], 0
		mov     byte ptr[esi + 4], 0
		mov		vm, ebx
		mov _esp, esp
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
		mov		[esp + 36], eax
		mov     eax, [edi + 16]
		mov cmd_id, eax
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
	if (id == SetRoundTime)
	{
		*(int*)(*(int*)eSystem::pMugenDataPointer + 0x12768) = 45 * 60;
	}

}

bool eScriptProcessor::IsCommandValid(int id)
{
	return id >= SetRoundTime;
}
