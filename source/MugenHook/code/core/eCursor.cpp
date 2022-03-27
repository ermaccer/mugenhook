#include "eCursor.h"
#include <iostream>
#include "..\mugenhook\eCommonHooks.h"
#include "..\mugenhook\base.h"
#include "..\core\eSettingsManager.h"
#include "..\mugen\System.h"

int eCursor::Player1_Row;
int eCursor::Player1_Column;
int eCursor::Player1_Character;
int eCursor::Player1_RandomCharacter;
int eCursor::Player1_Selected;
int eCursor::Player1_Turns;

int eCursor::Player2_Row;
int eCursor::Player2_Column;
int eCursor::Player2_Character;
int eCursor::Player2_RandomCharacter;
int eCursor::Player2_Selected;
int eCursor::Player2_Turns;

int eCursor::pCursorEax;

void eCursor::Init()
{
	Player1_Row = 0;
	Player1_Column = 0;
	Player1_Character = 0;
	Player1_RandomCharacter = 0;
	Player1_Selected = 0;
	Player1_Turns = 0;

	Player2_Row = 0;
	Player2_Column = 0;
	Player2_Character = 0;
	Player2_RandomCharacter = 0;
	Player2_Selected = 0;
	Player2_Turns = 0;
	pCursorEax = 0;

	InjectHook(0x406E51, HookCursorPointer, PATCH_JUMP);
	InjectHook(0x409196, HookCursorMoveNegative, PATCH_JUMP);
	InjectHook(0x409207, HookCursorMovePositive, PATCH_JUMP);
	InjectHook(0x409307, HookCursorMoveDown, PATCH_JUMP);
	InjectHook(0x409288, HookCursorMoveUp, PATCH_JUMP);
}

void __declspec(naked) eCursor::HookCursorPointer()
{
	static int pCursorJmp = 0x406E56;
	_asm
	{
		mov pCursorEax, eax
		add eax, 14864
		pushad
	}

	Update();
	eCommonHooks::ProcessSelectScreen();

	_asm
	{
		popad
		jmp pCursorJmp
	}
}

void __declspec(naked) eCursor::HookCursorMoveNegative()
{
	static int c_neg_jmp = 0x40919D;

	_asm
	{
		mov edx, [esp + 40]
		mov [ebp + 0],  ecx
		pushad
	}

	eCommonHooks::ProcessCursorNegative();
	_asm
	{
		popad
		jmp c_neg_jmp
	}

}

void __declspec(naked) eCursor::HookCursorMovePositive()
{
	static int c_pos_jmp = 0x40920E;

	_asm
	{
		mov eax, [esp + 20]
		mov[ebp + 0], ecx
		pushad
	}

	eCommonHooks::ProcessCursorPositive();
	_asm
	{
		popad
		jmp c_pos_jmp
	}
}

void __declspec(naked) eCursor::HookCursorMoveDown()
{
	static int c_pos_down_jmp = 0x40930D;

	_asm
	{
		mov  eax, [esp + 20]
		mov [edx], ecx
		pushad
	}

	eCommonHooks::ProcessCursorDown();
	_asm
	{
		popad
		jmp c_pos_down_jmp
	}
}

void __declspec(naked) eCursor::HookCursorMoveUp()
{
	static int c_pos_up_jmp = 0x40928E;

	_asm
	{
		mov  eax, [esp + 20]
		mov[edx], ecx
		pushad
	}

	eCommonHooks::ProcessCursorUp();
	_asm
	{
		popad
		jmp c_pos_up_jmp
	}
}

void eCursor::Update()
{

	Player1_Character = *(int*)(pCursorEax + 14846 + 18 + 4 + 4);
	Player1_RandomCharacter = *(int*)(pCursorEax + 14846 + 18 + 4 + 4 + 12);
	Player1_Row = (*(int*)(pCursorEax + 14846 + 18 + 4));
	Player1_Column = (*(int*)(pCursorEax + 14864));

	if (*(int*)(pCursorEax + 14846 - 214) == 2) {
		if (*(int*)((pCursorEax + 14846 + 42)) == 2) {
			Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4));
			Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4));
			Player1_RandomCharacter = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4 + 12));
			Player1_Column = (*(int*)(pCursorEax + 14864 + 48));
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 214) == 3) {
		if (*(int*)((pCursorEax + 14846 + 42)) == 2) {
			Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4));
			Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4));
			Player1_RandomCharacter = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4 + 12));
			Player1_Column = (*(int*)(pCursorEax + 14864 + 48));
			if (*(int*)((pCursorEax + 14846 + 48 + 42)) == 2) {
				Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4));
				Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4 + 4));
				Player1_RandomCharacter = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4 + 4 + 12));
				Player1_Column = (*(int*)(pCursorEax + 14864 + 48 + 48));
			}
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 214) == 4) {
		if (*(int*)((pCursorEax + 14846 + 42)) == 2) {
			Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4));
			Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4));
			Player1_RandomCharacter = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4 + 12));
			Player1_Column = (*(int*)(pCursorEax + 14864 + 48));
			if (*(int*)((pCursorEax + 14846 + 48 + 42)) == 2) {
				Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4));
				Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4 + 4));
				Player1_RandomCharacter = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4 + 4 + 12));
				Player1_Column = (*(int*)(pCursorEax + 14864 + 48 + 48));
				if (*(int*)((pCursorEax + 14846 + 48 + 48 + 42)) == 2) {
					Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 48 + 48 + 18 + 4));
					Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 48 + 48 + 18 + 4 + 4));
					Player1_RandomCharacter = (*(int*)(pCursorEax + 14846 + 48 + 48 + 48 + 18 + 4 + 4 + 12));
					Player1_Column = (*(int*)(pCursorEax + 14864 + 48 + 48 + 48));
				}
			}

		}
	}


	Player1_Selected = (*(int*)(pCursorEax + 14592));
	Player1_Turns = (*(int*)(pCursorEax + 14592 + 36));

	Player2_Selected = (*(int*)(pCursorEax + 14592 + 4));
	Player2_Turns = (*(int*)(pCursorEax + 14592 + 36 + 116));


	Player2_Row = (*(int*)(pCursorEax + 14864 + 0xC0 + 4));
	Player2_Character = *(int*)(pCursorEax + 14864 + 0xC0 + 4 + 4);
	Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0));

	if (*(int*)(pCursorEax + 14846 - 98) == 2) {
		if (*(int*)((pCursorEax + 14846 + 0xC0 + 42)) == 2) {
			Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4));
			Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4 + 4));
			Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48));
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 98) == 3) {
		if (*(int*)((pCursorEax + 14846 + 0xC0 + 42)) == 2) {
			Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4));
			Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4 + 4));
			Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48));
			if (*(int*)((pCursorEax + 14846 + 0xC0 + 48 + 42)) == 2) {
				Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4));
				Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4 + 4));
				Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48 + 48));
			}
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 98) == 4) {
		if (*(int*)((pCursorEax + 14846 + 0xC0 + 42)) == 2) {
			Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4));
			Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4 + 4));
			Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48));
			if (*(int*)((pCursorEax + 14846 + 0xC0 + 48 + 42)) == 2) {
				Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4));
				Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4 + 4));
				Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48 + 48));
				if (*(int*)((pCursorEax + 14846 + 0xC0 + 48 + 48 + 42)) == 2) {
					Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 48 + 18 + 4));
					Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 48 + 18 + 4 + 4));
					Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48 + 48 + 48));
				}
			}
		}
	}
}

void eCursor::StoreCursor()
{
	if (eCursor::Player1_Character > -1)
	{
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2DC) = Player1_Row;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2E4) = Player1_Column;
	}
	if (eCursor::Player2_Character > -1)
	{
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2E0) = Player2_Row;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2E8) = Player2_Column;
	}
}

void eCursor::PopCursor()
{
	*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2DC) = eSystem::p1_cursor_startcell[0];
	*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2E4) = eSystem::p1_cursor_startcell[1];

	*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2E0) = eSystem::p2_cursor_startcell[0];
	*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x2E8) = eSystem::p2_cursor_startcell[1];
}
