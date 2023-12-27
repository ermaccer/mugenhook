#pragma once

enum eTextAlign {
	ALIGN_RIGHT = -1,
	ALIGN_CENTER,
	ALIGN_LEFT,
};

struct eTextParams
{
	float rotateY;
	float field_4;
	float m_xPos;
	float m_yPos;
	float m_scaleY;
	float m_scaleX;
	char field_18;
	char field_19;
	char field_1A;
	char field_1B;
	char field_1C;
	char field_1D;
	char field_1E;
	char field_1F;
	float m_letterSpacingY;
	float m_letterSpacingX;
	char field_28;
	char field_29;
	char field_2A;
	char field_2B;
	char field_2C;
	char field_2D;
	char field_2E;
	char field_2F;
	char field_30;
	char field_31;
	char field_32;
	char field_33;
	char field_34;
	char field_35;
	char field_36;
	char field_37;
	char field_38;
	char field_39;
	char field_3A;
	char field_3B;
	float field_3C;
	float m_skew;
	char field_44;
	char field_45;
	char field_46;
	char field_47;
	char field_48;
	char field_49;
	char field_4A;
	char field_4B;
	int m_alpha;
	float field_50;
	char field_54;
	char field_55;
	char field_56;
	char field_57;
	char field_58;
	char field_59;
	char field_5A;
	char field_5B;
	int field_5C;
};


// fonts
int LoadFont(char* file, int param);

// drawing
void Draw2DTextDebug(char* text, int font, int x, int y, int unk, int unk2, int r, int g, int b);
void Draw2DTextReal(char* text, int font, eTextParams* params, int unk, int unk2, int r, int g, int b);

void Draw2DText(char* text, int font, eTextParams* params,  int r, int g, int b, int align);