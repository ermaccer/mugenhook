#include "..\mugenhook\base.h"
#include "System.h"
#include "Sprite.h"

void DisplayExplod(MugenExplod * config)
{
	Call<0x4101F0, MugenExplod*>(config);
}

int RequestSprites(FILE * sff, int * spritesID)
{
	return CallAndReturn<int, 0x467B30, FILE*, int*>(sff, spritesID);
}

int DrawSprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	return CallAndReturn<int, 0x411C00, int, int, int, int, int, float, float>(a1, a2, a3, a4, a5, a6, a7);
}

void ScaleSprites(int ptr, float a1, float a2)
{
	Call<0x412120, int, float, float>(ptr, a1, a2);
}

int SpriteExists(int sprite)
{
	return CallAndReturn<int, 0x466840, int>(sprite);
}

int LoadSFFFile(char * file)
{
	return CallAndReturn<int, 0x467510, const char*>(file);
}

void __declspec(naked) readExplodConfig(char * name, int source, MugenExplod * explod)
{
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov edi, source

		mov esi, explod 
		push edi
		mov eax, name
		mov edx, 0x4117D0
		call edx

		mov esp, ebp
		pop ebp
		ret
	}
}

/*
EXPLOD CALL

Registers:

ESP + 4 - some float
ESP - as above
ECX - sprite data
EDI - explods pointer

EBX - data + 0x29e4

*/

void __declspec(naked) tesft()
{

	static float val = 1.0f;
	_asm {
		fld val
		fstp [esp]
		fstp [esp + 4]
	}
}


int __declspec(naked) test::draw_test(int sprite_pointer, int data, MugenExplod* config)
{
	static int res = 0;
	static float val = 1.0f;

	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE
	
		// params

		mov eax, config
		mov ebx, eax


		mov ecx, sprite_pointer
		mov edi, data

		push 0
		push 0
		push ecx
		push edi
	
		mov eax, 0x411C00
		call eax
	
		mov esp, ebp
		pop ebp
		ret
	}
}

int __declspec(naked) test::display(int sprite_pointer, int id)
{
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE
		
		mov eax, sprite_pointer
		mov ebx, id

		mov edx, 0x412430
		call edx

		mov esp, ebp
		pop ebp
		ret
	}
}
