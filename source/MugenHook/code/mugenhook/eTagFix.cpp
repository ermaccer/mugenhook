#include "eTagFix.h"
#include "base.h"
#include "..\core\eSettingsManager.h"

int eTagFix::m_pParam;
int eTagFix::m_nFighters;

void eTagFix::Init()
{
	m_pParam = 0;
	m_nFighters = 0;
	if (eSettingsManager::bEnableTagFix)
	{
		Patch<int>(0x406DA0 + 8, (int)eTagFix::Hook);
		InjectHook(0x455961, eTagFix::HookGameModeCommand, PATCH_JUMP);
	}

}

void __declspec(naked) eTagFix::Hook()
{
	static int tag_mode_jump = 0x406411;
	m_nFighters = eSettingsManager::iTagSelectableFighters;
	_asm {
		mov eax, [esp + 68]
		mov eax, [eax + 36]
		mov[eax + 8], 2
		mov[eax + 16], 0
		mov edx, m_nFighters
		mov[eax + 12], edx
		jmp tag_mode_jump
	}

}

void __declspec(naked) eTagFix::HookGameModeCommand()
{
	static int tag_command_jump = 0x455BFC;

	_asm {
		lea eax, [esp + 344]
		mov m_pParam, eax
	}

	if (*(char*)(m_pParam) == 't' && *(char*)(m_pParam + 1) == 'a' &&
		*(char*)(m_pParam + 2) == 'g' && *(char*)(m_pParam + 3) == 0)
	{
		_asm
		{
			mov eax, [esp + 1268]
			add edi, ebx
			mov[esp + 48], 2
			mov[eax], edi
			jmp tag_command_jump
		}
	}

	else if (*(char*)(m_pParam) == 't' && *(char*)(m_pParam + 1) == 'u' &&
		*(char*)(m_pParam + 2) == 'r' && *(char*)(m_pParam + 3) == 'n' &&
		*(char*)(m_pParam + 4) == 's' && *(char*)(m_pParam + 5) == 0)
	{
		_asm
		{
			mov eax, [esp + 1268]
			add edi, ebx
			mov[esp + 48], 3
			mov[eax], edi
			jmp tag_command_jump
		}
	}

}

