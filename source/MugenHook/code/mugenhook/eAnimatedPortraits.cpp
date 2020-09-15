#include "eAnimatedPortraits.h"
#include "..\core\eAirReader.h"
#include "..\core\eLog.h"
#include "..\mugen\System.h"
#include "..\core\eSettingsManager.h"
#include "..\mugen\Sprite.h"
#include "..\core\eCursor.h"
#include "..\mugenhook\eCommonHooks.h"

#include "eSelectScreenManager.h"


std::vector<ePortraitEntry> eAnimatedPortraits::AnimationTable;
std::vector<eFrameEntry> eAnimatedPortraits::FrameTable;

int eAnimatedPortraits::iFrameCounter_p1;
int eAnimatedPortraits::iFrameCounter_p2;

int eAnimatedPortraits::iSelectCounter_p1;
int eAnimatedPortraits::iSelectCounter_p2;

int eAnimatedPortraits::iTickCounter_p1;
int eAnimatedPortraits::iTickCounter_p2;

int eAnimatedPortraits::pFrameTablePointer;





void eAnimatedPortraits::Init()
{
	iFrameCounter_p1 = 0;
	iFrameCounter_p2 = 0;

	iSelectCounter_p1 = 0;
	iSelectCounter_p2 = 0;

	iTickCounter_p1 = eSystem::GetTimer();
	iTickCounter_p2 = eSystem::GetTimer();

	pFrameTablePointer = 0;

	eLog::PushMessage(__FUNCTION__, "Initialize\n");

	if (eSettingsManager::bHookAnimatedPortraits)
		eAnimatedPortraits::ReadFile("cfg\\animatedPortraits.dat");


	eAnimatedPortraits::ReadFramesFile("cfg\\frameLoader.dat");
	InjectHook(0x404CE2, HookRequestSprites, PATCH_JUMP);
	InjectHook(0x406FF3, HookDisplaySprites, PATCH_CALL);


}

void eAnimatedPortraits::ReadFile(const char * file)
{

	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		eLog::PushMessage(__FUNCTION__, "Failed! Tried to open: %s\n", file);
		eLog::PushError();
	}


	if (pFile)
	{
		eLog::PushMessage(__FUNCTION__, "Success! Reading: %s\n", file);

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
				eLog::PushMessage("eAirReader::ReadData ", "Reading %s\n", strAirName.c_str());
				eAirManager.push_back(reader);

			}
		}
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", AnimationTable.size());
		fclose(pFile);
	}
}

void eAnimatedPortraits::ReadFramesFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		eLog::PushMessage(__FUNCTION__, "Failed! Tried to open: %s\n", file);
		eLog::PushError();
	}

	if (pFile)
	{
		eLog::PushMessage(__FUNCTION__, "Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iGroupID = 0;
			if (sscanf(szLine, "%d", &iGroupID) == 1)
			{
				int iIndex = 0;

				sscanf(szLine, "%d %d", &iGroupID, &iIndex);

				// create entry
				eFrameEntry frame = { iGroupID, iIndex };

				FrameTable.push_back(frame);
			}
		}
		pFrameTablePointer = (int)&FrameTable[0];
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", FrameTable.size());
		fclose(pFile);
	}
}

void eAnimatedPortraits::Process()
{
	eAirReader AIR_Reader;
	eAirEntry Animation;
	int iAnimEntry;

	if (!eSettingsManager::bEnableSelectAnimations)
	{
		eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

		// get anims
		AIR_Reader = GetAIRFromName(GetCellFName(eCursor::Player1_Row, eCursor::Player1_Column));
		iAnimEntry = FindPortraitEntry(eCursor::Player1_Row, eCursor::Player1_Column);
		int multipiler = 0;
		if ((CharactersArray[eCursor::Player1_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
		{
			if (CharactersArray[eCursor::Player1_Character].CurrentVariation > 1)
			{
				multipiler = 100 * (CharactersArray[eCursor::Player1_Character].CurrentVariation - 1);
			}

		}

		Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
		// set scale
		if (eSettingsManager::bEnableAnimationScale) {
			*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX; // p1.face.scale
			*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
		}


		// loop
		if (iFrameCounter_p1 > Animation.MaxFrames - 1)
		{
			if (Animation.IsLooping)
				iFrameCounter_p1 = Animation.LoopStart - 1;
			else
				iFrameCounter_p1 = 0;
		}

		// set sprites
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x80C) = Animation.vAnimData[iFrameCounter_p1].Group; // p1.face group
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810) = Animation.vAnimData[iFrameCounter_p1].Index; // p1.face index

																												   // perform animation
		if (eSystem::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iFrameCounter_p1].Frametime) return;
		iFrameCounter_p1++;

		// reset timer
		iTickCounter_p1 = eSystem::GetTimer();
	}
	else
	{
		if (!eCursor::Player1_Selected) {

			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			// get anims
			AIR_Reader = GetAIRFromName(GetCellFName(eCursor::Player1_Row, eCursor::Player1_Column));
			iAnimEntry = FindPortraitEntry(eCursor::Player1_Row, eCursor::Player1_Column);
			int multipiler = 0;
			if ((CharactersArray[eCursor::Player1_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[eCursor::Player1_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[eCursor::Player1_Character].CurrentVariation - 1);
				}

			}

			Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);

			// set scale
			if (eSettingsManager::bEnableAnimationScale) {
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX; // p1.face.scale
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
			}


			// loop
			if (iFrameCounter_p1 > Animation.MaxFrames - 1)
			{
				if (Animation.IsLooping)
					iFrameCounter_p1 = Animation.LoopStart - 1;
				else
					iFrameCounter_p1 = 0;
			}

			// set sprites
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x80C) = Animation.vAnimData[iFrameCounter_p1].Group; // p1.face group
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810) = Animation.vAnimData[iFrameCounter_p1].Index; // p1.face index


																													   // perform animation
			if (eSystem::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iFrameCounter_p1].Frametime) return;
			iFrameCounter_p1++;

			// reset timer
			iTickCounter_p1 = eSystem::GetTimer();

			// fix bug where any character select animation would be stuck at last frame
			iSelectCounter_p1 = 0;

		}
		else
		{
			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			AIR_Reader = GetAIRFromName(GetCellFName(eCursor::Player1_Row, eCursor::Player1_Column));
			iAnimEntry = FindPortraitEntry(eCursor::Player1_Row, eCursor::Player1_Column);
			int multipiler = 0;
			if ((CharactersArray[eCursor::Player1_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[eCursor::Player1_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[eCursor::Player1_Character].CurrentVariation - 1);
				}

			}

			Animation = AIR_Reader.GetAnimation(AnimationTable[iAnimEntry].WinAnimationID + multipiler);

			// set scale
			if (eSettingsManager::bEnableAnimationScale) {
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX; // p1.face.scale
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
			}


			// freeze animation
			if (iSelectCounter_p1 > Animation.MaxFrames - 1)
			{
				if (Animation.IsLooping)
					iSelectCounter_p1 = Animation.LoopStart - 1;
				else
				    iSelectCounter_p1 = Animation.MaxFrames - 1;
			}

			// set sprites
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x80C) = Animation.vAnimData[iSelectCounter_p1].Group;
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810) = Animation.vAnimData[iSelectCounter_p1].Index;

			if (eSystem::GetTimer() - iTickCounter_p1 <= Animation.vAnimData[iSelectCounter_p1].Frametime) return;
			iSelectCounter_p1++;
			iTickCounter_p1 = eSystem::GetTimer();
		}
	}
}

void eAnimatedPortraits::ProcessP2()
{
	eAirReader reader;
	eAirEntry animation, alt_anim;
	bool m_bOnPlayerOne = false;
	int  iMaxFrames;
	int iAnimEntry;

	if (eCursor::Player1_Row == eCursor::Player2_Row  && eCursor::Player1_Column == eCursor::Player2_Column) m_bOnPlayerOne = true;

	if (!eSettingsManager::bEnableSelectAnimations)
	{
		reader = GetAIRFromName(GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column));
		iAnimEntry = FindPortraitEntry(eCursor::Player2_Row, eCursor::Player2_Column);
		eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

		int multipiler = 0;
		if ((CharactersArray[eCursor::Player2_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
		{
			if (CharactersArray[eCursor::Player2_Character].CurrentVariation > 1)
			{
				multipiler = 100 * (CharactersArray[eCursor::Player2_Character].CurrentVariation - 1);
			}

		}
	
		animation = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
		alt_anim = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationAlternateID + multipiler);


		if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims) iMaxFrames = alt_anim.MaxFrames - 1;
		else iMaxFrames = animation.MaxFrames - 1;

		if (eSettingsManager::bEnableAnimationScale)
		{
			*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 4) = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
			*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 8) = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
		}

		if (iFrameCounter_p2 > iMaxFrames)
		{
			if (animation.IsLooping || alt_anim.IsLooping)
			{
				if (animation.IsLooping)
					iFrameCounter_p2 = animation.LoopStart - 1;
				else if (alt_anim.IsLooping)
					iFrameCounter_p2 = alt_anim.LoopStart - 1;
			}
			else
				iFrameCounter_p2 = 0;
		}


		if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims)
		{
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iFrameCounter_p2].Group;
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iFrameCounter_p2].Index;
		}
		else {
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 4) = animation.vAnimData[iFrameCounter_p2].Group;
			*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 8) = animation.vAnimData[iFrameCounter_p2].Index;
		}


		if (eSystem::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iFrameCounter_p2].Frametime) return;
		iFrameCounter_p2++;
		iTickCounter_p2 = eSystem::GetTimer();
	}
	else {
		if (!eCursor::Player2_Selected)
		{
			reader = GetAIRFromName(GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column));
			iAnimEntry = FindPortraitEntry(eCursor::Player2_Row, eCursor::Player2_Column);
			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			int multipiler = 0;
			if ((CharactersArray[eCursor::Player2_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[eCursor::Player2_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[eCursor::Player2_Character].CurrentVariation - 1);
				}

			}

			animation = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + multipiler);
			alt_anim = reader.GetAnimation(AnimationTable[iAnimEntry].SelectAnimationAlternateID + multipiler);


			if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims) iMaxFrames = alt_anim.MaxFrames - 1;
			else iMaxFrames = animation.MaxFrames - 1;

			if (eSettingsManager::bEnableAnimationScale)
			{
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 4) = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 8) = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
			}


			
			if (iFrameCounter_p2 > iMaxFrames)
			{
				if (animation.IsLooping || alt_anim.IsLooping)
				{
					if (animation.IsLooping)
						iFrameCounter_p2 = animation.LoopStart - 1;
					else if (alt_anim.IsLooping)
						iFrameCounter_p2 = alt_anim.LoopStart - 1;
				}
				else
				   iFrameCounter_p2 = 0;
			}

			if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims)
			{
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iFrameCounter_p2].Group;
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iFrameCounter_p2].Index;
			}
			else {
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 4) = animation.vAnimData[iFrameCounter_p2].Group;
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 8) = animation.vAnimData[iFrameCounter_p2].Index;
			}


			if (eSystem::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iFrameCounter_p2].Frametime) return;
			iFrameCounter_p2++;
			iTickCounter_p2 = eSystem::GetTimer();
			iSelectCounter_p2 = 0;
		}
		else
		{
			reader = GetAIRFromName(GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column));
			iAnimEntry = FindPortraitEntry(eCursor::Player2_Row, eCursor::Player2_Column);

			eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

			int multipiler = 0;
			if ((CharactersArray[eCursor::Player2_Character].CharacterFlag & CHAR_FLAG_VARIATIONS))
			{
				if (CharactersArray[eCursor::Player2_Character].CurrentVariation > 1)
				{
					multipiler = 100 * (CharactersArray[eCursor::Player2_Character].CurrentVariation - 1);
				}

			}


			animation = GetAIRFromName(GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column)).GetAnimation(AnimationTable[iAnimEntry].WinAnimationID + multipiler);
			alt_anim = GetAIRFromName(GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column)).GetAnimation(AnimationTable[iAnimEntry].WinAnimationAlternateID + multipiler);


			if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims) iMaxFrames = alt_anim.MaxFrames - 1;
			else iMaxFrames = animation.MaxFrames - 1;


			if (iSelectCounter_p2 > iMaxFrames)
			{
				if (animation.IsLooping || alt_anim.IsLooping)
				{
					if (animation.IsLooping)
						iSelectCounter_p2 = animation.LoopStart - 1;
					else if (alt_anim.IsLooping)
						iSelectCounter_p2 = alt_anim.LoopStart - 1 ;
				}
				else
					iSelectCounter_p2 = iMaxFrames;
			}

			if (eSettingsManager::bEnableAnimationScale)
			{
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 4) = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
				*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40 + 0xD4 + 8) = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
			}

			if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims)
			{
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 4) = alt_anim.vAnimData[iSelectCounter_p2].Group;
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 8) = alt_anim.vAnimData[iSelectCounter_p2].Index;
			}
			else
			{
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 4) = animation.vAnimData[iSelectCounter_p2].Group;
				*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810 + 0xD0 + 8) = animation.vAnimData[iSelectCounter_p2].Index;
			}


			if (eSystem::GetTimer() - iTickCounter_p2 <= animation.vAnimData[iSelectCounter_p2].Frametime) return;
			iSelectCounter_p2++;
			iTickCounter_p2 = eSystem::GetTimer();
		}
	}
}

void eAnimatedPortraits::ResetFrameCounter(int player)
{
	if (player == 1)
		iFrameCounter_p1 = 0;
	else
		iFrameCounter_p2 = 0;
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

void __declspec(naked) eAnimatedPortraits::HookRequestSprites()
{
	static int req_jmp = 0x404CEF;
	_asm
	{
		push    pFrameTablePointer
		call 	RequestSprites
		pushad
		popad
		jmp req_jmp;
	}
}

int eAnimatedPortraits::HookDisplaySprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	if (eSettingsManager::bHookAnimatedPortraits)
	{
		Process();
		ProcessP2();
	}

	eCommonHooks::ProcessCharacterSpriteEvent();

	return  DrawSprites(a1, a2, a3, a4, a5, a6, 0);
}

