#include "eTagFix.h"
#include "base.h"
#include "..\core\eSettingsManager.h"
#include "..\mugen\System.h"

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
		Nop(0x4245C2, 6);
		InjectHook(0x4245C2, eTagFix::HookTeamLifeFix, PATCH_JUMP);
		Nop(0x4245EA, 6);
		InjectHook(0x4245EA, eTagFix::HookTeamLifeFixP2, PATCH_JUMP);
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

void  __declspec(naked) eTagFix::HookTeamLifeFix()
{
	static int teamlife_jmp = 0x4245CA;
	static int teamlife_jmp_fail = 0x4245DA;
	_asm {

		cmp[esi + 0x50AC], 1
		je ok
		cmp[esi + 0x50AC], 2
		je ok
		jmp teamlife_jmp_fail

		ok:
		jmp teamlife_jmp
	}
}

void __declspec(naked) eTagFix::HookTeamLifeFixP2()
{
	static int teamlife2_jmp = 0x4245F2;
	static int teamlife2_jmp_fail = 0x424604;
	_asm {

		cmp[esi + 0x6B88], 1
		je ok
		cmp[esi + 0x6B88], 2
		je ok
		jmp teamlife2_jmp_fail

		ok :
		jmp teamlife2_jmp
	}
}

