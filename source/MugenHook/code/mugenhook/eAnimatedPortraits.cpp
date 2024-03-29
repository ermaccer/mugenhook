#include "eAnimatedPortraits.h"
#include "..\core\eAirReader.h"
#include "..\core\eLog.h"
#include "..\mugen\System.h"
#include "..\core\eSettingsManager.h"
#include "..\mugen\Sprite.h"
#include "..\core\eCursor.h"
#include "..\mugenhook\eCommonHooks.h"

#include "eCustomCursorManager.h"
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

bool eAnimatedPortraits::bReadyToDrawSprite;

int eAnimatedPortraits::pCurrentPlayerSprite;
int eAnimatedPortraits::pCurrentPlayerSpriteP2;

int eAnimatedPortraits::iCurrentPlayerDrawID;

void eAnimatedPortraits::Init()
{
	iFrameCounter_p1 = 0;
	iFrameCounter_p2 = 0;

	iSelectCounter_p1 = 0;
	iSelectCounter_p2 = 0;

	iTickCounter_p1 = eSystem::GetTimer();
	iTickCounter_p2 = eSystem::GetTimer();

	pFrameTablePointer = 0;
	pCurrentPlayerSprite = 0;
	bReadyToDrawSprite = false;

	eLog::PushMessage(__FUNCTION__, "Initialize\n");

	if (eSettingsManager::bHookAnimatedPortraits)
		eAnimatedPortraits::ReadFile("cfg\\animatedPortraits.dat");


	eAnimatedPortraits::ReadFramesFile("cfg\\frameLoader.dat");
	InjectHook(0x404CE2, HookRequestSprites, PATCH_JUMP);
	InjectHook(0x406FF3, HookDisplaySprites, PATCH_CALL);
	Nop(0x406F95, 6);
	InjectHook(0x406F95, HookGrabPlayerIDForDrawing, PATCH_JUMP);

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
				int parsed = 0;
				int iColumnID = 0, iGroupID = 0, iGroupP2ID = 0, iSelectGroupID = 0, iSelectGroupP2ID = 0, iCharacterID = 0;

				float fSpriteScaleX, fSpriteScaleY;
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					parsed = sscanf(szLine, "%d %d %d %d %d %s %f %f", &iCharacterID, &iGroupID, &iGroupP2ID, &iSelectGroupID, &iSelectGroupP2ID, &airPath, &fSpriteScaleX, &fSpriteScaleY);
				else
					parsed = sscanf(szLine, "%d %d %d %d %d %d %s %f %f", &iRowID, &iColumnID, &iGroupID, &iGroupP2ID, &iSelectGroupID, &iSelectGroupP2ID, &airPath, &fSpriteScaleX, &fSpriteScaleY);

				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
				{
					if (parsed > 8)
					{
						eLog::PushMessage(__FUNCTION__, "Current operation mode is set to ID, but the data line seems to be made for Row/Column!");
						eLog::PushError();
					}
				}

				std::string strAirName(airPath, strlen(airPath));
				// create entry

				ePortraitEntry entry = { iRowID, iColumnID, iGroupID, iGroupP2ID, iSelectGroupID,
					iSelectGroupP2ID, strAirName,fSpriteScaleX,fSpriteScaleY,iCharacterID};

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
	eAirReader* AIR_Reader;
	eAirEntry* Animation;
	int iAnimEntry;

	MugenExplod* characterExplod = (MugenExplod*)(*(int*)eSystem::pMugenResourcesPointer + 0x808);

	if (!eCursor::Player1_Selected || !eSettingsManager::bEnableSelectAnimations) {

		eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;


		if (eCursor::Player1_Character > -1)
		{

			std::string air;
			// handle random
			if (CharactersArray[eCursor::Player1_Character].ID == -2)
			{
				air = GetCellFNameID(GetCharacterIDFromSprite(pCurrentPlayerSprite));
			}
			else
			{
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					air = GetCellFNameID(CharactersArray[eCursor::Player1_Character].ID);
				else
					air = GetCellFName(eCursor::Player1_Row, eCursor::Player1_Column);
			}




			// get anims
			AIR_Reader = GetAIRFromName(air);

			// handle random
			if (CharactersArray[eCursor::Player1_Character].ID == -2)
			{
				iAnimEntry = FindPortraitEntryID(GetCharacterIDFromSprite(pCurrentPlayerSprite));
			}
			else
			{
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					iAnimEntry = FindPortraitEntryID(CharactersArray[eCursor::Player1_Character].ID);
				else
					iAnimEntry = FindPortraitEntry(eCursor::Player1_Row, eCursor::Player1_Column);
			}




			if (AIR_Reader)
			{
				int cur_var = CharactersArray[eCursor::Player1_Character].CurrentVariation - 1;
				int var_offset = 0;
				if (cur_var >= 1)
					var_offset = 100 * cur_var;

				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					var_offset = 0;

				Animation = AIR_Reader->GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + var_offset);

				if (Animation)
				{
					// set scale
					if (eSettingsManager::bEnableAnimationScale) {
						characterExplod->scaleX = AnimationTable[iAnimEntry].SpritesScaleX;
						characterExplod->scaleY = AnimationTable[iAnimEntry].SpritesScaleY;
						//*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x40) = AnimationTable[iAnimEntry].SpritesScaleX // p1.face.scale
						//*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x44) = AnimationTable[iAnimEntry].SpritesScaleY; // p1.face.scale
					}


					// loop
					if (eCursor::Player1_Character == -2)
						iFrameCounter_p1 = 0;
					else
					{
						if (iFrameCounter_p1 > Animation->MaxFrames - 1)
						{
							if (Animation->IsLooping)
								iFrameCounter_p1 = Animation->LoopStart - 1;
							else
								iFrameCounter_p1 = 0;
						}
					}

					// set sprites
					characterExplod->index = Animation->vAnimData[iFrameCounter_p1].Index;
					characterExplod->group = Animation->vAnimData[iFrameCounter_p1].Group;
					//*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x80C) = Animation->vAnimData[iFrameCounter_p1].Group; // p1.face group
					//*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x810) = Animation->vAnimData[iFrameCounter_p1].Index; // p1.face index


																															   // perform animation
					if (eSystem::GetTimer() - iTickCounter_p1 <= Animation->vAnimData[iFrameCounter_p1].Frametime) return;
					if (!(eCursor::Player1_Character == -2))
						iFrameCounter_p1++;

					// reset timer
					iTickCounter_p1 = eSystem::GetTimer();

					// fix bug where any character select animation would be stuck at last frame
					iSelectCounter_p1 = 0;
				}
			}
		}
	}
	else
	{
		if (eSettingsManager::bEnableSelectAnimations)
		{
			eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

			if (eCursor::Player1_Character > -1)
			{

				std::string air;
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
				{
					air = GetCellFNameID(eCursor::SelectionP1.ID);
				}

				else
					air = GetCellFName(eCursor::Player1_Row, eCursor::Player1_Column);
		

				// get anims
				AIR_Reader = GetAIRFromName(air);

				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
				{
					iAnimEntry = FindPortraitEntryID(eCursor::SelectionP1.ID);
				}
				else
					iAnimEntry = FindPortraitEntry(eCursor::Player1_Row, eCursor::Player1_Column);

				if (AIR_Reader)
				{
					int cur_var = CharactersArray[eCursor::Player1_Character].CurrentVariation - 1;
					int var_offset = 0;
					if (cur_var >= 1)
						var_offset = 100 * cur_var;


					if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
						var_offset = 0;

					Animation = AIR_Reader->GetAnimation(AnimationTable[iAnimEntry].WinAnimationID + var_offset);

					if (Animation)
					{
						// set scale
						if (eSettingsManager::bEnableAnimationScale) {
							characterExplod->scaleX = AnimationTable[iAnimEntry].SpritesScaleX;
							characterExplod->scaleY = AnimationTable[iAnimEntry].SpritesScaleY;
						}


						// freeze animation
						if (iSelectCounter_p1 > Animation->MaxFrames - 1)
						{
							if (Animation->IsLooping)
								iSelectCounter_p1 = Animation->LoopStart - 1;
							else
								iSelectCounter_p1 = Animation->MaxFrames - 1;
						}

						// set sprites
						characterExplod->group = Animation->vAnimData[iSelectCounter_p1].Group;
						characterExplod->index = Animation->vAnimData[iSelectCounter_p1].Index;

						if (eSystem::GetTimer() - iTickCounter_p1 <= Animation->vAnimData[iSelectCounter_p1].Frametime) return;
						iSelectCounter_p1++;
						iTickCounter_p1 = eSystem::GetTimer();

					}
					
				}
			}
		}
	}
}

void eAnimatedPortraits::ProcessP2()
{
	eAirReader* AIR_Reader;
	eAirEntry* animation, *alt_anim;


	MugenExplod* characterExplod = (MugenExplod*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0xD8);

	bool m_bOnPlayerOne = false;

	int  iMaxFrames;
	int  iAnimEntry;

	if (eCursor::Player1_Row == eCursor::Player2_Row  
		&& eCursor::Player1_Column == eCursor::Player2_Column) 
		m_bOnPlayerOne = true;

	if (!eCursor::Player2_Selected || !eSettingsManager::bEnableSelectAnimations)
	{
		eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
		if (eCursor::Player2_Character > -1)
		{

			std::string air;
			// handle random
			if (CharactersArray[eCursor::Player2_Character].ID == -2)
			{
				air = GetCellFNameID(GetCharacterIDFromSprite(pCurrentPlayerSpriteP2));
			}
			else
			{
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					air = GetCellFNameID(CharactersArray[eCursor::Player2_Character].ID);
				else
					air = GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column);
			}




			// get anims
			AIR_Reader = GetAIRFromName(air);

			// handle random
			if (CharactersArray[eCursor::Player2_Character].ID == -2)
			{
				iAnimEntry = FindPortraitEntryID(GetCharacterIDFromSprite(pCurrentPlayerSpriteP2));
			}
			else
			{
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					iAnimEntry = FindPortraitEntryID(CharactersArray[eCursor::Player2_Character].ID);
				else
					iAnimEntry = FindPortraitEntry(eCursor::Player2_Row, eCursor::Player2_Column);
			}


			if (AIR_Reader)
			{
				int cur_var = CharactersArray[eCursor::Player2_Character].CurrentVariation - 1;
				int var_offset = 0;
				if (cur_var >= 1)
					var_offset = 100 * cur_var;

				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
					var_offset = 0;

				animation = AIR_Reader->GetAnimation(AnimationTable[iAnimEntry].SelectAnimationID + var_offset);
				alt_anim = AIR_Reader->GetAnimation(AnimationTable[iAnimEntry].SelectAnimationAlternateID + var_offset);

				if (animation && alt_anim)
				{
					if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims) 
						iMaxFrames = alt_anim->MaxFrames - 1;
					else 
						iMaxFrames = animation->MaxFrames - 1;

					if (eSettingsManager::bEnableAnimationScale)
					{
						characterExplod->scaleX = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
						characterExplod->scaleY = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
					}

					if (iFrameCounter_p2 > iMaxFrames)
					{
						if (animation->IsLooping || alt_anim->IsLooping)
						{
							if (animation->IsLooping)
								iFrameCounter_p2 = animation->LoopStart - 1;
							else if (alt_anim->IsLooping)
								iFrameCounter_p2 = alt_anim->LoopStart - 1;
						}
						else
							iFrameCounter_p2 = 0;
					}


					if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims)
					{
						characterExplod->group = alt_anim->vAnimData[iFrameCounter_p2].Group;
						characterExplod->index = alt_anim->vAnimData[iFrameCounter_p2].Index;
					}
					else {
						characterExplod->group = alt_anim->vAnimData[iFrameCounter_p2].Group;
						characterExplod->index = alt_anim->vAnimData[iFrameCounter_p2].Index;
					}

					if (eSystem::GetTimer() - iTickCounter_p2 <= animation->vAnimData[iFrameCounter_p2].Frametime) return;
					iFrameCounter_p2++;
					iTickCounter_p2 = eSystem::GetTimer();
					iSelectCounter_p2 = 0;
				}
			}
		}	
	}
	else
	{
		if (eSettingsManager::bEnableSelectAnimations)
		{
			eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

			if (eCursor::Player2_Character > -1)
			{
				std::string air;
				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
				{
						air = GetCellFNameID(eCursor::SelectionP2.ID);
				}

				else
					air = GetCellFName(eCursor::Player2_Row, eCursor::Player2_Column);



				// get anims
				AIR_Reader = GetAIRFromName(air);

				if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
				{
					iAnimEntry = FindPortraitEntryID(eCursor::SelectionP2.ID);
				}
				else
					iAnimEntry = FindPortraitEntry(eCursor::Player2_Row, eCursor::Player2_Column);



				if (AIR_Reader)
				{
					int cur_var = CharactersArray[eCursor::Player2_Character].CurrentVariation - 1;
					int var_offset = 0;
					if (cur_var >= 1)
						var_offset = 100 * cur_var;

					if (eSettingsManager::iAnimatedPortraitsOperationType == MODE_CHAR_ID)
						var_offset = 0;

					animation = AIR_Reader->GetAnimation(AnimationTable[iAnimEntry].WinAnimationID + var_offset);
					alt_anim = AIR_Reader->GetAnimation(AnimationTable[iAnimEntry].WinAnimationAlternateID + var_offset);

					if (animation && alt_anim)
					{
						if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims)
							iMaxFrames = alt_anim->MaxFrames - 1;
						else
							iMaxFrames = animation->MaxFrames - 1;

						if (iSelectCounter_p2 > iMaxFrames)
						{
							if (animation->IsLooping || alt_anim->IsLooping)
							{
								if (animation->IsLooping)
									iSelectCounter_p2 = animation->LoopStart - 1;
								else if (alt_anim->IsLooping)
									iSelectCounter_p2 = alt_anim->LoopStart - 1;
							}
							else
								iSelectCounter_p2 = iMaxFrames;
						}

						if (eSettingsManager::bEnableAnimationScale)
						{
							characterExplod->scaleX = AnimationTable[iAnimEntry].SpritesScaleX; // p2.face.scale
							characterExplod->scaleY = AnimationTable[iAnimEntry].SpritesScaleY; // p2.face.scale
						}

						if (m_bOnPlayerOne && eSettingsManager::bEnableAlternateAnims)
						{
							characterExplod->group = alt_anim->vAnimData[iSelectCounter_p2].Group;
							characterExplod->index = alt_anim->vAnimData[iSelectCounter_p2].Index;
						}
						else {
							characterExplod->group = alt_anim->vAnimData[iSelectCounter_p2].Group;
							characterExplod->index = alt_anim->vAnimData[iSelectCounter_p2].Index;
						}

						if (eSystem::GetTimer() - iTickCounter_p2 <= animation->vAnimData[iSelectCounter_p2].Frametime) return;
						iSelectCounter_p2++;
						iTickCounter_p2 = eSystem::GetTimer();
					}
				}
			}
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

int eAnimatedPortraits::FindPortraitEntryID(int id)
{
	int iFind = 0;
	for (unsigned int i = 0; i < AnimationTable.size(); i++)
	{
		if (AnimationTable[i].CharID == id) 
		{
			iFind = i;
			break;
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

std::string eAnimatedPortraits::GetCellFNameID(int id)
{
	std::string strFind;
	for (unsigned int i = 0; i < AnimationTable.size(); i++)
	{
		if (AnimationTable[i].CharID == id) 
		{
				strFind = AnimationTable[i].AirFileName;
				break;
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
#ifdef IMPROVE_SPRITE_DISPLAY
	if (iCurrentPlayerDrawID == 0)
	{
		if (eCursor::Player1_Character > -1)
		{
			if (CharactersArray[eCursor::Player1_Character].ID > -1)
				a2 = CharactersArray[eCursor::Player1_Character].SpritePointer;
		}

		pCurrentPlayerSprite = a2;
	}
	else if (iCurrentPlayerDrawID == 4)
	{
		if (!(eSystem::GetGameplayMode() == MODE_WATCH))
		{
			if (eCursor::Player2_Character > -1)
			{
				if (CharactersArray[eCursor::Player2_Character].ID > -1)
					a2 = CharactersArray[eCursor::Player2_Character].SpritePointer;
			}

		}

		pCurrentPlayerSpriteP2 = a2;
	}
#else
	if (iCurrentPlayerDrawID == 0)
		pCurrentPlayerSprite = a2;
	if (iCurrentPlayerDrawID == 4)
		pCurrentPlayerSpriteP2 = a2;
#endif


#ifdef MULTICHAR_DEBUG
	if (eSelectScreenManager::m_bDrawExtraCharacterP1)
	{
		eMugenCharacterInfo* chr = GetCharInfo(eCursor::SelectionData[0][eSelectScreenManager::m_nSelectScreenCurrentCharacterDraw].ID);
		if (chr)
			a2 = chr->SpritePointer;
	}

#endif // MULTICHAR_DEBUG	

	if (eSettingsManager::bHookAnimatedPortraits)
	{
		Process();
		ProcessP2();
	}

	eCommonHooks::ProcessCharacterSpriteEvent();

	return  DrawSprites(a1, a2, a3, a4, a5, a6, 0);
}

int eAnimatedPortraits::HookDisplaySprites2(int a1, int a2, int a3, int a4, int a5, float a6, float a7)
{
	return  DrawSprites(a1, a2, a3, a4, a5, a6, 0);
}

void __declspec(naked) eAnimatedPortraits::HookGrabPlayerIDForDrawing()
{
	static int jmp_playerid_next = 0x406F9B;
	_asm {
		mov		iCurrentPlayerDrawID, ecx
		mov     eax, [esi + 0x438]
		jmp		jmp_playerid_next
	}
}

