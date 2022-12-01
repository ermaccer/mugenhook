#include "eSlidingPortraits.h"
#include "..\mugen\System.h"
#include <filesystem>
#include "..\..\IniReader.h"
#include "..\core\eLog.h"
#include "..\core\eSettingsManager.h"
#include "eMenuManager.h"

int   eSlidingPortraits::m_tTimer;
int   eSlidingPortraits::m_tTimerP2;
float eSlidingPortraits::p1_portrait_slide_speed;
float eSlidingPortraits::p2_portrait_slide_speed;
float eSlidingPortraits::p1_portrait_slide_max_dist;
float eSlidingPortraits::p2_portrait_slide_max_dist;
int   eSlidingPortraits::p1_portrait_axis;
int   eSlidingPortraits::p2_portrait_axis;
int   eSlidingPortraits::portrait_slide_reset_on_move;
bool  eSlidingPortraits::m_bStopSliding;
bool  eSlidingPortraits::m_bStopSlidingP2;
bool  eSlidingPortraits::m_bRequireRefresh;


void eSlidingPortraits::Init()
{
	m_tTimer = 0;
	m_tTimerP2 = 0;
	p1_portrait_slide_speed = 0;
	p2_portrait_slide_speed = 0;
	p1_portrait_slide_max_dist = 0;
	p2_portrait_slide_max_dist = 0;
	p1_portrait_axis = 0;
	p2_portrait_axis = 0;
	m_bStopSliding = 0;
	m_bStopSlidingP2 = 0;
	m_bRequireRefresh = false;

	if (eSettingsManager::bEnableSlidePortraits)
	{
		char* motif;
		if (std::filesystem::exists("data\\mugen.cfg"))
		{
			CIniReader mugenConfig("data\\mugen.cfg");
			motif = mugenConfig.ReadString("Options", "motif", 0);
			if (motif)
			{
				CIniReader system(motif);

				// controls speed of slide 
				p1_portrait_slide_speed = system.ReadFloat("Select Info", "p1.portrait.slide.speed", 0.0);
				p2_portrait_slide_speed = system.ReadFloat("Select Info", "p2.portrait.slide.speed", 0.0);

				// defines axis
				p1_portrait_axis = system.ReadInteger("Select Info", "p1.portrait.axis", AXIS_X);
				p2_portrait_axis = system.ReadInteger("Select Info", "p2.portrait.axis", AXIS_X);

				// how much can it go offset
				p1_portrait_slide_max_dist = system.ReadFloat("Select Info", "p1.portrait.slide.max.dist", 0.0);
				p2_portrait_slide_max_dist = system.ReadFloat("Select Info", "p2.portrait.slide.max.dist", 0.0);

				portrait_slide_reset_on_move = system.ReadInteger("Select Info", "portrait.slide.reset", 0);

				if (p1_portrait_axis != AXIS_X || p2_portrait_axis != AXIS_X)
				{
					eLog::PushMessage(__FUNCTION__, "ERROR: Unknown value for axis! Defaulting to AXIS_X (0)\n");
					p1_portrait_axis = 0;
					p2_portrait_axis = 0;
				}
			}
		}
		else
		{
			eLog::PushMessage(__FUNCTION__, "Failed! Could not open mugen.cfg!\n");
			eLog::PushError();
		}


	}

	
}

void eSlidingPortraits::Process()
{
	if (eMenuManager::m_bIsInSelectScreen) {

		// update per tick
		if (eSystem::GetTimer() - m_tTimer <= 1) return;
		m_tTimer = eSystem::GetTimer();

		if (*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x24) >= p1_portrait_slide_max_dist)
			m_bStopSliding = true;

		if (!m_bStopSliding)
			*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x24) += p1_portrait_slide_speed;

	}
}

void eSlidingPortraits::ProcessP2()
{
	if (eMenuManager::m_bIsInSelectScreen) {
		// update per tick
		if (eSystem::GetTimer() - m_tTimerP2 <= 1) return;
		m_tTimerP2 = eSystem::GetTimer();

		if (*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0xD4 + 0x24 + 4) <= p2_portrait_slide_max_dist)
			m_bStopSlidingP2 = true;

		if (!m_bStopSlidingP2)
			*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x24 + 0xD4 + 4) += p2_portrait_slide_speed;
	}
}

void eSlidingPortraits::Reset()
{
	eSlidingPortraits::ResetPlayer(1);
	eSlidingPortraits::ResetPlayer(2);
}

void eSlidingPortraits::ResetPlayer(int player)
{
	if (player == 1)
	{
		m_bStopSliding = false;
		*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x24) = eSystem::p1_face_offset[0];
		*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0x24 + 4) = eSystem::p1_face_offset[1];
	}
	else
	{
		m_bStopSlidingP2 = false;
		*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0xD4 + 0x24 + 4) = eSystem::p2_face_offset[0];
		*(float*)(*(int*)eSystem::pMugenResourcesPointer + 0x808 + 0xD4 + 0x24 + 4 + 4) = eSystem::p2_face_offset[1];
	}
}
