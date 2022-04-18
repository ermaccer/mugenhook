#pragma once

struct eSelection {
	int ID;
	int Who;
};

class eCursor {
public:
	static int Player1_Row;
	static int Player1_Column;
	static int Player1_Character;
	static int Player1_RandomCharacter;
	static int Player1_Selected;
	static int Player1_Turns;

	static int Player2_Row;
	static int Player2_Column;
	static int Player2_Character;
	static int Player2_RandomCharacter;
	static int Player2_Selected;
	static int Player2_Turns;

	static eSelection Selection;
	static eSelection SelectionP1;
	static eSelection SelectionP2;
	static int pCursorEax;


	static void Init();
	static void HookCursorPointer();
	static void HookCursorMoveNegative();
	static void HookCursorMovePositive();
	static void HookCursorMoveDown();
	static void HookCursorMoveUp();
	static void HookSelection();
	static void Update();

	static void StoreCursor();
	static void PopCursor();

	static void UpdateRandomSelect();

};
