#pragma once
#include "..\mugen\Draw.h"
#include "eScriptProcessor.h"

#define TEXTDRAW_NUM  8
#define TEXTDRAW_MAX_ARGS 3

enum eTextDrawFlags {
	ETextDrawFlags_NoBackground = 1 << 0,
	ETextDrawFlags_Blink = 1 << 1,
	ETextDrawFlags_Matrix = 1 << 2,
	ETextDrawFlags_Relative = 1 << 3,
};

enum eTextDrawArgType {
	ETextDrawArgType_Integer = 1,
	ETextDrawArgType_Float
};

struct eTextDrawArg {
	int integerData;
	float floatData;
};


struct eTextDrawConfig {
	bool m_bLifeBarAffected;

	unsigned char m_red;
	unsigned char m_green;
	unsigned char m_blue;
	unsigned char m_reserved;

	int m_nLayerno;
	int m_nAlign;
	int m_nFont;
	int m_nDuration;

	int m_nArgsNum;
	eTextDrawArgType m_eArgTypes[TEXTDRAW_MAX_ARGS];
	eTextDrawArg m_nArgs[TEXTDRAW_MAX_ARGS];

	eTextDrawFlags flags;
};

struct eTextDrawEntry {
	bool m_bIsActive;

	int  m_nActiveTime;
	int  m_nTickUpdate;

	int  m_nTimer;

	int  m_pFont;
	char m_szDisplayText[512] = {};
	int  m_pOwner;


	eTextDrawConfig m_tConfig;
	eTextParams m_tParams;
};


class eTextDrawProcessor {
public:
	static eTextDrawEntry ms_tTextDraws[TEXTDRAW_NUM][8];

	static void InitTextDraw();

	static void SetTextDraw(int owner, int id, int tID, eTextParams& params, eTextDrawConfig& drawConfig, char* text);
	static void DisableTextDraw(int id, int tID);
	static void DrawTextDraws(int layerNo);
	static void UpdateTextDraws();
	static void UpdateOneTextDraw(eTextDrawEntry* ent);
};