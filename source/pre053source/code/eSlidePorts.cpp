#include "eSlidePorts.h"
#include "eMugen.h"
#include <iostream>
#include <filesystem>
#include "..\IniReader.h"
#include "eCursorManager.h"
#include "eSettingsManager.h"
#include "eMagicBoxes.h"
#include "eLog.h"

static int iTimer = *(int*)(*(int*)0x5040E8 + 0x11E98);
static int iTimer_P2 =*(int*)(*(int*)0x5040E8 + 0x11E98);
float p1_portrait_slide_speed;
float p2_portrait_slide_speed;
float p1_portrait_slide_max_dist;
float p2_portrait_slide_max_dist;
int p1_portrait_axis;
int p2_portrait_axis;
float p1_face_offset[2];
float p2_face_offset[2];
bool bInSelectScreen;
bool bStopSliding, bStopSlidingP2;

enum eAxis {
	AXIS_X,
	// todo
	AXIS_Y,
	AXIS_BOTH
};

void eSlidePorts::ReadSettings()
{
	char* motif;
	if (std::experimental::filesystem::exists("data\\mugen.cfg"))
	{
		CIniReader mugenConfig("data\\mugen.cfg");
		motif = mugenConfig.ReadString("Options", "motif", 0);
		if (motif)
		{
			CIniReader system(motif);

			// save og values
			sscanf(system.ReadString("Select Info", "p1.face.offset", 0), "%f,%f", &p1_face_offset[0], &p1_face_offset[1]);
			sscanf(system.ReadString("Select Info", "p2.face.offset", 0), "%f,%f", &p2_face_offset[0], &p2_face_offset[1]);

			// controls speed of slide 
			p1_portrait_slide_speed = system.ReadFloat("Select Info", "p1.portrait.slide.speed", 0.0);
			p2_portrait_slide_speed = system.ReadFloat("Select Info", "p2.portrait.slide.speed", 0.0);

			// defines axis
			p1_portrait_axis = system.ReadInteger("Select Info", "p1.portrait.axis", AXIS_X);
			p2_portrait_axis = system.ReadInteger("Select Info", "p2.portrait.axis", AXIS_X);

			// how much can it go offset
			p1_portrait_slide_max_dist = system.ReadFloat("Select Info", "p1.portrait.slide.max.dist", 0.0);
			p2_portrait_slide_max_dist = system.ReadFloat("Select Info", "p2.portrait.slide.max.dist", 0.0);

			if (p1_portrait_axis != AXIS_X || p2_portrait_axis != AXIS_X)
			{
				Log->PushMessage(false, "eSlidePorts::ReadSettings() | ERROR: Unknown value for axis! Defaulting to AXIS_X (0)\n");
				p1_portrait_axis = 0;
				p2_portrait_axis = 0;
			}
		}
	}
	else
	{
		Log->PushMessage(false,"eSlidePorts::ReadSettings() | Failed! Could not open mugen.cfg!\n");
		Log->PushError();
	}

}

void eSlidePorts::Update()
{
	if (bInSelectScreen){

		// update per tick
		if (eMugenManager::GetTimer() - iTimer <= 1) return;
		iTimer = eMugenManager::GetTimer();

		if (*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x24) >= p1_portrait_slide_max_dist)
				bStopSliding = true;

		if (!bStopSliding) 
			*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x24) += p1_portrait_slide_speed;

	}
}

void eSlidePorts::UpdateP2()
{
	if (bInSelectScreen) {
		// update per tick
		if (eMugenManager::GetTimer() - iTimer_P2 <= 1) return;
		iTimer_P2 = eMugenManager::GetTimer();

		if (*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0xD4 + 0x24 + 4) <= p2_portrait_slide_max_dist)
			bStopSlidingP2 = true;

		if (!bStopSlidingP2)
			*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x24 + 0xD4 + 4) += p2_portrait_slide_speed;
	}
}

int eSlidePorts::HookSelectCase()
{
	bInSelectScreen = true;
	return ((int(__cdecl*)())0x408A80)();;
}

int eSlidePorts::HookMenuCase()
{

	if (SettingsMgr->bHookMagicBoxes)
	{
		eMagicBoxes::GetBox();
	}

	if (SettingsMgr->bEnableSlidePortraits)
	{
		bInSelectScreen = false;
		bStopSliding = false;
		bStopSlidingP2 = false;
		// reset pos
		*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x24) = p1_face_offset[0];
		*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0x24 + 4) = p1_face_offset[1];

		*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0xD4 + 0x24 + 4) = p2_face_offset[0];
		*(float*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x808 + 0xD4 + 0x24 + 4 + 4) = p2_face_offset[1];
	}


	return ((int(__cdecl*)())0x429C20)();;
}

int eSlidePorts::HookGameLoop()
{

	return  ((int(__cdecl*)())0x412FA0)();
}
