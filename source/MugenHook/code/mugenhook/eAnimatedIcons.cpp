#include "eAnimatedIcons.h"
#include "eSelectScreenManager.h"
#include "..\core\eSettingsManager.h"
#include "..\core\eAirReader.h"
#include "..\core\eLog.h"
#include "eMenuManager.h"

std::vector<eAnimIconEntry> eAnimatedIcons::m_vAnimatedIcons;
std::vector<int> eAnimatedIcons::m_vUsedCharacters;
bool eAnimatedIcons::m_bScanRequiredForAIcons;
int eAnimatedIcons::m_nIconcounter;


void eAnimatedIcons::Init()
{
	m_bScanRequiredForAIcons = true;
	m_nIconcounter = 0;
	if (eSettingsManager::bHookAnimatedIcons)
	     ReadFile("cfg\\animIcons.dat");
	
}


void eAnimatedIcons::ReadFile(const char * file)
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

				int iCharacterID, iAnimationID, iFlags;


				sscanf(szLine, "%d %d %s %d", &iCharacterID, &iAnimationID, &airPath, &iFlags);

				std::string strAirName(airPath, strlen(airPath));
				// create entry

				eAnimIconEntry entry = { iCharacterID, iAnimationID, strAirName, iFlags };

				m_vAnimatedIcons.push_back(entry);

				eAirReader reader;
				reader.Open(strAirName.c_str());
				reader.ReadData();
				eAirManager.push_back(reader);

			}
		}
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", m_vAnimatedIcons.size());
		fclose(pFile);
	}
}

void eAnimatedIcons::FlagCharacters()
{
	if (m_bScanRequiredForAIcons)
	{
		eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

		for (int i = 0; i < eSystem::iRows * eSystem::iColumns; i++)
		{
			for (unsigned int a = 0; a < m_vAnimatedIcons.size(); a++)
				if (CharactersArray[i].ID == m_vAnimatedIcons[a].CharacterID)
				{
					CharactersArray[i].CharacterFlag |= CHAR_FLAG_ANIM_ICON;
					CharactersArray[i].ExtraFlags |= m_vAnimatedIcons[a].Flags;
					m_vUsedCharacters.push_back(i);
				}
		}
	}
	m_bScanRequiredForAIcons = false;

}

int eAnimatedIcons::FindIconEntry(int charID)
{
	int iFind = 0;

	for (unsigned int i = 0; i < m_vAnimatedIcons.size(); i++)
	{
		if (m_vAnimatedIcons[i].CharacterID == charID)
		{
			iFind = i;
			break;
		}
	}
	return iFind;
}

void eAnimatedIcons::Animate(eMugenCharacterInfo* character)
{
	int iIcon = FindIconEntry(character->ID);

	eAirReader* AIR_Reader;
	eAirEntry* Animation;

	AIR_Reader = GetAIRFromName(m_vAnimatedIcons[iIcon].AirFileName);

	Animation = AIR_Reader->GetAnimation(m_vAnimatedIcons[iIcon].AnimationID);


	if (character->ExtraFlags & EXTRA_FLAG_VAR_SPECIAL_CONTROLLER && character->CharacterFlag & CHAR_FLAG_VARIATIONS)
	{

		if (character->CurrentIconIndex > Animation->MaxFrames - 1)
			character->CurrentIconIndex = Animation->MaxFrames - 1;

		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x230 + 4) = Animation->vAnimData[character->CurrentIconIndex].Group;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x230 + 8) = Animation->vAnimData[character->CurrentIconIndex].Index;

		if (eSystem::GetTimer() - character->IconTimer <= Animation->vAnimData[character->CurrentIconIndex].Frametime) return;
		character->CurrentIconIndex++;

		// reset timer
		character->IconTimer = eSystem::GetTimer();
	}
	else
	{
		if (character->ExtraFlags & EXTRA_FLAG_ANIM_PLAYS_ONCE)
		{
			if (character->CurrentIconIndex > Animation->MaxFrames - 1)
				character->CurrentIconIndex = Animation->MaxFrames - 1;
		}
		else
		{
			if (character->CurrentIconIndex > Animation->MaxFrames - 1)
				character->CurrentIconIndex = 0;
		}


		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x230 + 4) = Animation->vAnimData[character->CurrentIconIndex].Group;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x230 + 8) = Animation->vAnimData[character->CurrentIconIndex].Index;

		if (eSystem::GetTimer() - character->IconTimer <= Animation->vAnimData[character->CurrentIconIndex].Frametime) return;
		character->CurrentIconIndex++;

		// reset timer
		character->IconTimer = eSystem::GetTimer();
	}
	
}

void eAnimatedIcons::RefreshAnimationCounters()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;


	eAirReader* AIR_Reader;
	eAirEntry* Animation;

	for (int i = 0; i < eSystem::iColumns * eSystem::iRows; i++)
		for (unsigned int a = 0; a < m_vAnimatedIcons.size(); a++)
		{
			if (CharactersArray[i].ID == m_vAnimatedIcons[a].CharacterID)
			{
				if (CharactersArray[i].ExtraFlags & EXTRA_FLAG_VAR_SPECIAL_CONTROLLER)
				{
					// we want variation character to be on last image
					int iIcon = FindIconEntry(CharactersArray[i].ID);
					AIR_Reader = GetAIRFromName(m_vAnimatedIcons[iIcon].AirFileName);
					Animation = AIR_Reader->GetAnimation(m_vAnimatedIcons[iIcon].AnimationID);
					CharactersArray[i].CurrentIconIndex = Animation->MaxFrames - 1;
				}
				else
					CharactersArray[i].CurrentIconIndex = 0;

			}
		}
}

void eAnimatedIcons::ResetTimer()
{

}
