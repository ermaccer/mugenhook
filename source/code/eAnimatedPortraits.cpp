#include "eAnimatedPortraits.h"
#include "eMugen.h"
#include "eCursorManager.h"
#include "eAirReader.h"
#include "eMagicBoxes.h"
#include <iostream>
#include <vector>
#include "..\stdafx.h"

std::vector<ePortraitEntry> AnimationTable;
std::vector<eFrameEntry> FrameTable;
int iFrameCounter_p1, iSelectCounter_p1;
int iFrameCounter_p2, iSelectCounter_p2;
static int iTickCounter_p1 = eMugenManager::GetTimer();
static int iTickCounter_p2 = eMugenManager::GetTimer();
int pFrameTablePointer;
int iLoadSpritesJump = 0x404CEF;
bool bEnableDebug;
bool bEnableSelectAnims;
bool bEnableAltAnims;

void eAnimatedPortraits::Init()
{
	printf("eAnimatedPortraits::Init() | Initialize\n");
}

void eAnimatedPortraits::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
		printf("eAnimatedPortraits::ReadFile() | Failed! Tried to open: %s\n", file);

	if (pFile)
	{
		printf("eAnimatedPortraits::ReadFile() | Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iRowID = 0;
			if (sscanf(szLine, "%d", &iRowID) == 1)
			{
				char airPath[512];
				int iColumnID, iGroupID, iGroupP2ID, iMaxFrames, iFrameTime, iSelectGroupID,
					iSelectGroupP2ID, iSelectMaxFrames, iSelectFrametime, iAlternateGroup;

				sscanf(szLine, "%d %d %d %d %d %d %s", &iRowID, &iColumnID, &iGroupID, &iGroupP2ID, &iSelectGroupID, &iSelectGroupP2ID, &airPath);

				std::string strAirName(airPath, strlen(airPath));
				// create entry

				ePortraitEntry entry = { iRowID, iColumnID, iGroupID, iGroupP2ID, iSelectGroupID, 
					iSelectGroupP2ID, strAirName };

				AnimationTable.push_back(entry);

				eAirReader reader;
				reader.Open(strAirName.c_str());
				reader.ReadData();
				eAirManager.push_back(reader);

			}
		}
		printf("eAnimatedPortraits::ReadFile() | Read %d entries\n", AnimationTable.size());
		fclose(pFile);
	}
}

void eAnimatedPortraits::ReadFramesFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		printf("eAnimatedPortraits::ReadFramesFile() | Failed! Tried to open: %s\n", file);
	}

	if (pFile)
	{
		printf("eAnimatedPortraits::ReadFramesFile() | Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iGroupID = 0;
			if (sscanf(szLine, "%d", &iGroupID) == 1)
			{
				int iIndex;

				sscanf(szLine, "%d %d", &iGroupID, &iIndex);

				// create entry
				eFrameEntry frame = { iGroupID, iIndex };

				FrameTable.push_back(frame);
			}
		}
		pFrameTablePointer = (int)&FrameTable[0];
		printf("eAnimatedPortraits::ReadFramesFile() | Read %d entries\n", FrameTable.size());
		fclose(pFile);
	}
}

int eAnimatedPortraits::FindPortraitEntry(int row, int col)
{
	int iFind = 0;
	for (int i = 0; i < AnimationTable.size(); i++)
	{
		if (AnimationTable[i].RowID == row) {
			if (AnimationTable[i].ColumnID == col)
			{
				iFind = i;
				break;
			}
		}
	}
	return iFind;
}

std::string eAnimatedPortraits::GetCellFName(int row, int col)
{
	std::string strFind;
	for (int i = 0; i < AnimationTable.size(); i++)
	{
		if (AnimationTable[i].RowID == row) {
			if (AnimationTable[i].ColumnID == col)
			{
				strFind = AnimationTable[i].AirFileName;
				break;
			}
		}
	}
	return strFind;
}

int eAnimatedPortraits::HookDisplaySprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	ProcessSelectScreen();
	ProcessSelectScreenP2();
	return  DrawSprites(a1, a2, a3, a4, a5, a6, a7);
}


int eAnimatedPortraits::GetFrameTablePointer()
{
	printf("%x", (int)&FrameTable[0]);
	return (int)&FrameTable[0];
}

void eAnimatedPortraits::ProcessSelectScreen()
{
	eAirReader AIR_Reader;
	eAirEntry Animation;
	int iAnimEntry;

	if (!bEnableSelectAnims)
	{	
		// get anims
		AIR_Reader = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(1), eCursorManager::GetPlayerColumn(1)));
		iAnimEntry = FindPortraitEntry(eCursorManager::GetPlayerRow(1), eCursorManager::GetPlayerColumn(1));
		Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID);

		// loop
		if (iFrameCounter_p1 >  Animation.MaxFrames - 1) iFrameCounter_p1 = 0;

		// set sprites
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x80C) = Animation.vAnimData[iFrameCounter_p1].Group; // p1.face group
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810) = Animation.vAnimData[iFrameCounter_p1].Index; // p1.face index

		// perform animation
		if (eMugenManager::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iFrameCounter_p1].Frametime) return;
		iFrameCounter_p1++;

		// reset timer
		iTickCounter_p1 = eMugenManager::GetTimer();
	}
	else 
	{
		if (!eCursorManager::GetPlayerSelected(1)) {
			// get anims
			AIR_Reader = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(1), eCursorManager::GetPlayerColumn(1)));
			iAnimEntry = FindPortraitEntry(eCursorManager::GetPlayerRow(1), eCursorManager::GetPlayerColumn(1));
			Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID);

			// loop
			if (iFrameCounter_p1 >  Animation.MaxFrames - 1) iFrameCounter_p1 = 0;

			// set sprites
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x80C) = Animation.vAnimData[iFrameCounter_p1].Group; // p1.face group
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810) = Animation.vAnimData[iFrameCounter_p1].Index; // p1.face index


			// perform animation
			if (eMugenManager::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iFrameCounter_p1].Frametime) return;
			iFrameCounter_p1++;

			// reset timer
			iTickCounter_p1 = eMugenManager::GetTimer();
			
			// fix bug where any character select animation would be stuck at last frame
			iSelectCounter_p1 = 0;

		}
		else
		{
			AIR_Reader = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(1), eCursorManager::GetPlayerColumn(1)));
			iAnimEntry = FindPortraitEntry(eCursorManager::GetPlayerRow(1), eCursorManager::GetPlayerColumn(1));
			Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].WinAnimationID);

			// freeze animation
			if (iSelectCounter_p1 >Animation.MaxFrames - 1) iSelectCounter_p1 = Animation.MaxFrames - 1;

			// set sprites
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x80C) = Animation.vAnimData[iSelectCounter_p1].Group;
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810) = Animation.vAnimData[iSelectCounter_p1].Index;

			if (eMugenManager::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iSelectCounter_p1].Frametime) return;
			iSelectCounter_p1++;
			iTickCounter_p1 = eMugenManager::GetTimer();
		}
	}

	
	


}

void eAnimatedPortraits::ProcessSelectScreenP2()
{
	eAirReader reader;
	eAirEntry animation, alt_anim;
	bool bOnp1;
	int  iMaxFrames;
	int AnimEntry_p2;

	if (eCursorManager::GetPlayerRow(1) == eCursorManager::GetPlayerRow(2) && eCursorManager::GetPlayerColumn(1) == eCursorManager::GetPlayerColumn(2)) bOnp1 = true;

	if (!bEnableSelectAnims)
	{
		reader = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2)));
		AnimEntry_p2 = FindPortraitEntry(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2));
		animation = reader.GetAnimation(AnimationTable[AnimEntry_p2].SelectAnimationID);
		alt_anim = reader.GetAnimation(AnimationTable[AnimEntry_p2].SelectAnimationAlternateID);

		if (bOnp1 && bEnableAltAnims) iMaxFrames = alt_anim.MaxFrames - 1;
		else iMaxFrames = animation.MaxFrames - 1;

		if (iFrameCounter_p2 >  iMaxFrames) iFrameCounter_p2 = 0;

		if (bOnp1 && bEnableAltAnims)
		{
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iFrameCounter_p2].Group;
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iFrameCounter_p2].Index;
		}
		else {
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 4) = animation.vAnimData[iFrameCounter_p2].Group;
			*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 8) = animation.vAnimData[iFrameCounter_p2].Index;
		}


		if (eMugenManager::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iFrameCounter_p2].Frametime) return;
		iFrameCounter_p2++;
		iTickCounter_p2 = eMugenManager::GetTimer();
	}
	else {
		if (!eCursorManager::GetPlayerSelected(2))
		{
			reader = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2)));
			AnimEntry_p2 = FindPortraitEntry(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2));
			animation = reader.GetAnimation(AnimationTable[AnimEntry_p2].SelectAnimationID);
			alt_anim = reader.GetAnimation(AnimationTable[AnimEntry_p2].SelectAnimationAlternateID);
			if (bOnp1 && bEnableAltAnims) iMaxFrames = alt_anim.MaxFrames - 1;
			else iMaxFrames = animation.MaxFrames - 1;

			if (iFrameCounter_p2 >  iMaxFrames) iFrameCounter_p2 = 0;

			if (bOnp1 && bEnableAltAnims)
			{
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iFrameCounter_p2].Group;
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iFrameCounter_p2].Index;
			}
			else {
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 4) = animation.vAnimData[iFrameCounter_p2].Group;
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 8) = animation.vAnimData[iFrameCounter_p2].Index;
			}


			if (eMugenManager::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iFrameCounter_p2].Frametime) return;
			iFrameCounter_p2++;
			iTickCounter_p2 = eMugenManager::GetTimer();
			iSelectCounter_p2 = 0;
		}
		else
		{
			reader = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2)));
			AnimEntry_p2 = FindPortraitEntry(*(int*)(eCursorManager::GetCursorEax() + 14864 + 0xC0 + 16), *(int*)(eCursorManager::GetCursorEax() + 14864 + 0xC0));
			animation = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2))).GetAnimation(AnimationTable[AnimEntry_p2].WinAnimationID);
			alt_anim = GetAIRFromName(GetCellFName(eCursorManager::GetPlayerRow(2), eCursorManager::GetPlayerColumn(2))).GetAnimation(AnimationTable[AnimEntry_p2].WinAnimationAlternateID);
			if (iSelectCounter_p2 >animation.MaxFrames - 1) iSelectCounter_p2 = animation.MaxFrames - 1;
			if (bOnp1 && bEnableAltAnims)
			{
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iSelectCounter_p2].Group;
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iSelectCounter_p2].Index;
			}
			else
			{
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 4) = animation.vAnimData[iSelectCounter_p2].Group;
				*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x810 + 0xD0 + 8) = animation.vAnimData[iSelectCounter_p2].Index;
			}


			if (eMugenManager::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iSelectCounter_p2].Frametime) return;
			iSelectCounter_p2++;
			iTickCounter_p2 = eMugenManager::GetTimer();
		}
	}

}

void eAnimatedPortraits::EnableDebug()
{
	bEnableDebug = true;
}

void eAnimatedPortraits::EnableSelectAnimations()
{
	bEnableSelectAnims = true;
}

void eAnimatedPortraits::EnableAltAnims()
{
	bEnableAltAnims = true;
}

void __declspec(naked) eAnimatedPortraits::HookRequestSprites(int a1, int a2)
{
	_asm 
	{
        push    pFrameTablePointer
		call 	RequestSprites
		pushad
	}

	_asm 
	{
		popad
		jmp iLoadSpritesJump
	}
}

