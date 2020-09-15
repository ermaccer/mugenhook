#include "eSettingsManager.h"
#include "eCursorManager.h"
#include "eAnimatedPortraits.h"
#include "eMugen.h"
#include "eLog.h"
#include "eAirReader.h"
#include <iostream>
#include <vector>
#include "eSlidePorts.h"
#include "eMagicBoxes.h"
#include "eVariationsManager.h"
#include "..\stdafx.h"


std::vector<ePortraitEntry> AnimationTable;
std::vector<eFrameEntry> FrameTable;
int iFrameCounter_p1, iSelectCounter_p1;
int iFrameCounter_p2, iSelectCounter_p2;
static int iTickCounter_p1 = *(int*)(*(int*)0x5040E8 + 0x11E98);
static int iTickCounter_p2 = *(int*)(*(int*)0x5040E8 + 0x11E98);
int pFrameTablePointer;
int iLoadSpritesJump = 0x404CEF;
int characterSprite = 0;
int cursorSprite = 0;

void eAnimatedPortraits::Init()
{
	Log->PushMessage(false, "eAnimatedPortraits::Init() | Initialize\n");
}

void eAnimatedPortraits::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		Log->PushMessage(false, "eAnimatedPortraits::ReadFile() | Failed! Tried to open: %s\n", file);
		Log->PushError();
	}


	if (pFile)
	{
		Log->PushMessage(false, "eAnimatedPortraits::ReadFile() | Success! Reading: %s\n", file);

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
				int iColumnID, iGroupID, iGroupP2ID, iSelectGroupID, iSelectGroupP2ID;

				float fSpriteScaleX, fSpriteScaleY;

				sscanf(szLine, "%d %d %d %d %d %d %s %f %f", &iRowID, &iColumnID, &iGroupID, &iGroupP2ID, &iSelectGroupID, &iSelectGroupP2ID, &airPath, &fSpriteScaleX, &fSpriteScaleY);

				std::string strAirName(airPath, strlen(airPath));
				// create entry

				ePortraitEntry entry = { iRowID, iColumnID, iGroupID, iGroupP2ID, iSelectGroupID,
					iSelectGroupP2ID, strAirName,fSpriteScaleX,fSpriteScaleY };

				AnimationTable.push_back(entry);

				eAirReader reader;
				reader.Open(strAirName.c_str());
				reader.ReadData();
				Log->PushMessage(false, "eAirReader::ReadData() | Reading %s\n", strAirName.c_str());
				eAirManager.push_back(reader);

			}
		}
		Log->PushMessage(false, "eAnimatedPortraits::ReadFile() | Read %d entries\n", AnimationTable.size());
		fclose(pFile);
	}
}

void eAnimatedPortraits::ReadFramesFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		Log->PushMessage(false, "eAnimatedPortraits::ReadFramesFile() | Failed! Tried to open: %s\n", file);
		Log->PushError();
	}

	if (pFile)
	{
		Log->PushMessage(false, "eAnimatedPortraits::ReadFramesFile() | Success! Reading: %s\n", file);

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
		Log->PushMessage(false, "eAnimatedPortraits::ReadFramesFile() | Read %d entries\n", FrameTable.size());
		fclose(pFile);
	}
}

int eAnimatedPortraits::FindPortraitEntry(int row, int col)
{
	int iFind = 0;
	for (unsigned int i = 0; i < AnimationTable.size(); i++)
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
	for (unsigned int i = 0; i < AnimationTable.size(); i++)
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
	if (SettingsMgr->bHookAnimatedPortraits)
	{
		ProcessSelectScreen();
		ProcessSelectScreenP2();
	}

	if (SettingsMgr->bEnableSlidePortraits)
	{
		eSlidePorts::UpdateP2();
		eSlidePorts::Update();
	}

	if (SettingsMgr->bHookMagicBoxes)
	{
		eMagicBoxes::UpdateMagicBoxes();
	}

	//characterSprite =


	//DrawSprites(a1, CharactersArray[TheCursor->Player2_Character].SpritePointer, a3, a4, a5, a6, a7);
	return  DrawSprites(a1, a2, a3, a4, a5, a6, a7);
}

int eAnimatedPortraits::HookCharSpritePriority(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	cursorSprite = DrawSprites(a1, a2, a3, a4, a5, a6, a7);
	//ScaleSprites(cursorSprite, 1.0, 1.0);

	return characterSprite;
}

void eAnimatedPortraits::HookCharSpritePrioritySecond(int a1, float a2, float a3)
{
	ScaleSprites(a1, a2, a3);
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

	if (!SettingsMgr->bEnableSelectAnimations)
	{
		eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

		// get anims
		AIR_Reader = GetAIRFromName(GetCellFName(TheCursor->Player1_Row, TheCursor->Player1_Column));
		iAnimEntry = FindPortraitEntry(TheCursor->Player1_Row, TheCursor->Player1_Column);
		int multipiler = 0;
		if (!(CharactersArray[TheCursor->Player1_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
		{
			if (CharactersArray[TheCursor->Player1_Character].CurrentVariation > 1)
			{
				multipiler = 100 * (CharactersArray[TheCursor->Player1_Character].CurrentVariation - 1);
			}

		}
		Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
		// set scale
		if (SettingsMgr->bEnableAnimationScale) {
			*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX; // p1.face.scale
			*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
		}


		// loop
		if (iFrameCounter_p1 >  Animation.MaxFrames - 1) iFrameCounter_p1 = 0;

		// set sprites
		*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x80C) = Animation.vAnimData[iFrameCounter_p1].Group; // p1.face group
		*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810) = Animation.vAnimData[iFrameCounter_p1].Index; // p1.face index

																												   // perform animation
		if (eMugenManager::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iFrameCounter_p1].Frametime) return;
		iFrameCounter_p1++;

		// reset timer
		iTickCounter_p1 = eMugenManager::GetTimer();
	}

	else
	{
		if (!TheCursor->Player1_Selected) {

			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			// get anims
			AIR_Reader = GetAIRFromName(GetCellFName(TheCursor->Player1_Row, TheCursor->Player1_Column));
			iAnimEntry = FindPortraitEntry(TheCursor->Player1_Row, TheCursor->Player1_Column);
			int multipiler = 0;
			if (!(CharactersArray[TheCursor->Player1_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[TheCursor->Player1_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[TheCursor->Player1_Character].CurrentVariation - 1);
				}

			}
			//printf("anim %d\n", AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
			Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);

			// set scale
			if (SettingsMgr->bEnableAnimationScale) {
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX; // p1.face.scale
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
			}


			// loop
			if (iFrameCounter_p1 >  Animation.MaxFrames - 1) iFrameCounter_p1 = 0;

			// set sprites
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x80C) = Animation.vAnimData[iFrameCounter_p1].Group; // p1.face group
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810) = Animation.vAnimData[iFrameCounter_p1].Index; // p1.face index


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
			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			AIR_Reader = GetAIRFromName(GetCellFName(TheCursor->Player1_Row, TheCursor->Player1_Column));
			iAnimEntry = FindPortraitEntry(TheCursor->Player1_Row, TheCursor->Player1_Column);
			int multipiler = 0;
			if (!(CharactersArray[TheCursor->Player1_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[TheCursor->Player1_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[TheCursor->Player1_Character].CurrentVariation - 1);
				}

			}
			//printf("Loading animation %d\n", AnimationTable[iAnimEntry].WinAnimationID + multipiler);
			Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].WinAnimationID + multipiler);

			// set scale
			if (SettingsMgr->bEnableAnimationScale) {
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX; // p1.face.scale
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
			}


			// freeze animation
			if (iSelectCounter_p1 >Animation.MaxFrames - 1) iSelectCounter_p1 = Animation.MaxFrames - 1;

			// set sprites
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x80C) = Animation.vAnimData[iSelectCounter_p1].Group;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810) = Animation.vAnimData[iSelectCounter_p1].Index;

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
	int iAnimEntry;

	if (TheCursor->Player1_Row == TheCursor->Player2_Row  && TheCursor->Player1_Column == TheCursor->Player2_Column) bOnp1 = true;

	if (!SettingsMgr->bEnableSelectAnimations)
	{
		reader = GetAIRFromName(GetCellFName(TheCursor->Player2_Row, TheCursor->Player2_Column));
		iAnimEntry = FindPortraitEntry(TheCursor->Player2_Row, TheCursor->Player2_Column);
		eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

		int multipiler = 0;
		if (!(CharactersArray[TheCursor->Player2_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
		{
			if (CharactersArray[TheCursor->Player2_Character].CurrentVariation > 1)
			{
				multipiler = 100 * (CharactersArray[TheCursor->Player2_Character].CurrentVariation - 1);
			}

		}

		animation = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
		alt_anim = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationAlternateID + multipiler);

		if (bOnp1 && SettingsMgr->bEnableAlternateAnims) iMaxFrames = alt_anim.MaxFrames - 1;
		else iMaxFrames = animation.MaxFrames - 1;

		if (SettingsMgr->bEnableAnimationScale)
		{
			*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 4) = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
			*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 8) = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
		}

		if (iFrameCounter_p2 >  iMaxFrames) iFrameCounter_p2 = 0;

		if (bOnp1 && SettingsMgr->bEnableAlternateAnims)
		{
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iFrameCounter_p2].Group;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iFrameCounter_p2].Index;
		}
		else {
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 4) = animation.vAnimData[iFrameCounter_p2].Group;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 8) = animation.vAnimData[iFrameCounter_p2].Index;
		}


		if (eMugenManager::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iFrameCounter_p2].Frametime) return;
		iFrameCounter_p2++;
		iTickCounter_p2 = eMugenManager::GetTimer();
	}
	else {
		if (!TheCursor->Player2_Selected)
		{
			reader = GetAIRFromName(GetCellFName(TheCursor->Player2_Row, TheCursor->Player2_Column));
			iAnimEntry = FindPortraitEntry(TheCursor->Player2_Row, TheCursor->Player2_Column);
			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			int multipiler = 0;
			if (!(CharactersArray[TheCursor->Player2_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[TheCursor->Player2_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[TheCursor->Player2_Character].CurrentVariation - 1);
				}

			}

			animation = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
			alt_anim = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationAlternateID + multipiler);
			if (bOnp1 && SettingsMgr->bEnableAlternateAnims) iMaxFrames = alt_anim.MaxFrames - 1;
			else iMaxFrames = animation.MaxFrames - 1;

			if (SettingsMgr->bEnableAnimationScale)
			{
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 4) = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 8) = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
			}


			if (iFrameCounter_p2 >  iMaxFrames) iFrameCounter_p2 = 0;

			if (bOnp1 && SettingsMgr->bEnableAlternateAnims)
			{
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iFrameCounter_p2].Group;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iFrameCounter_p2].Index;
			}
			else {
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 4) = animation.vAnimData[iFrameCounter_p2].Group;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 8) = animation.vAnimData[iFrameCounter_p2].Index;
			}


			if (eMugenManager::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iFrameCounter_p2].Frametime) return;
			iFrameCounter_p2++;
			iTickCounter_p2 = eMugenManager::GetTimer();
			iSelectCounter_p2 = 0;
		}
		else
		{
			reader = GetAIRFromName(GetCellFName(TheCursor->Player2_Row, TheCursor->Player2_Column));
			iAnimEntry = FindPortraitEntry(TheCursor->Player2_Row, TheCursor->Player2_Column);

			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			int multipiler = 0;
			if (!(CharactersArray[TheCursor->Player2_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[TheCursor->Player2_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[TheCursor->Player2_Character].CurrentVariation - 1);
				}

			}


			animation = GetAIRFromName(GetCellFName(TheCursor->Player2_Row, TheCursor->Player2_Column)).GetAnimation(AnimationTable[iAnimEntry].WinAnimationID + multipiler);
			alt_anim = GetAIRFromName(GetCellFName(TheCursor->Player2_Row, TheCursor->Player2_Column)).GetAnimation(AnimationTable[iAnimEntry].WinAnimationAlternateID + multipiler);
			if (iSelectCounter_p2 >animation.MaxFrames - 1) iSelectCounter_p2 = animation.MaxFrames - 1;

			if (SettingsMgr->bEnableAnimationScale)
			{
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 4) = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
				*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 8) = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
			}

			if (bOnp1 && SettingsMgr->bEnableAlternateAnims)
			{
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iSelectCounter_p2].Group;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iSelectCounter_p2].Index;
			}
			else
			{
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 4) = animation.vAnimData[iSelectCounter_p2].Group;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x810 + 0xD0 + 8) = animation.vAnimData[iSelectCounter_p2].Index;
			}


			if (eMugenManager::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iSelectCounter_p2].Frametime) return;
			iSelectCounter_p2++;
			iTickCounter_p2 = eMugenManager::GetTimer();
		}
	}
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
