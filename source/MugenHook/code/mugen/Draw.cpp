#include "Draw.h"
#include "..\mugenhook\base.h"

int LoadFont(char * file, int param)
{
	return CallAndReturn<int, 0x46D030, char*, int>(file, param);
}

void Draw2DTextDebug(char * text, int font, int x, int y, int unk, int unk2, int r, int g, int b)
{
	Call<0x46CE40, char*, int, int, int, int, int, int, int, int>(text, font, x, y, unk, unk2, r, g, b);
}

void Draw2DTextReal(char * text, int font, eTextParams * params, int unk, int unk2, int r, int g, int b)
{
	Call<0x46CA80, char*, int, eTextParams*, int, int, int, int, int>(text, font, params, unk, unk2, r, g, b);
}

void __declspec(naked) Draw2DText(char* text, int font, eTextParams* params, int r, int g, int b, int align)
{
	_asm {
		push ebp                
		mov ebp, esp           
		sub esp, __LOCAL_SIZE    

		mov     ecx, b
		mov		edx, g
		mov		eax, r

		push ecx // B

		mov ecx, 0
		
		push edx // G

		mov edx, align

		push eax // R

		push ecx // UNK2

		mov ecx, font 
			
		push edx // UNK

		//lea eax, Dst	
		push params		// PARAMS

		mov eax, text 

		push ecx		// FONT
		
		mov edx, 0x46CA80
		call edx

		mov esp, ebp           
		pop ebp
		ret
	}

}
