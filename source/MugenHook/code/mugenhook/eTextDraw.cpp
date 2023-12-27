#include "eTextDraw.h"
#include "..\mugen\Draw.h"
#include "..\mugen\System.h"
#include <iostream>

eTextDrawEntry  eTextDrawProcessor::ms_tTextDraws[TEXTDRAW_NUM][8];

void eTextDrawProcessor::InitTextDraw()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < TEXTDRAW_NUM; j++)
		{
			ms_tTextDraws[j][i] = {};
			ms_tTextDraws[j][i].m_nTimer = eSystem::GetTimer();
			ms_tTextDraws[j][i].m_nTickUpdate = 1;
		}
	}
}

void eTextDrawProcessor::SetTextDraw(int owner, int id, int tID, eTextParams& params, eTextDrawConfig& drawConfig, char* text)
{
	ms_tTextDraws[tID][id].m_nTimer = eSystem::GetTimer();
	ms_tTextDraws[tID][id].m_pOwner = owner;
	ms_tTextDraws[tID][id].m_tParams = params;
	ms_tTextDraws[tID][id].m_tConfig = drawConfig;
	sprintf(ms_tTextDraws[tID][id].m_szDisplayText, text);
	ms_tTextDraws[tID][id].m_nActiveTime = ms_tTextDraws[tID][id].m_tConfig.m_nDuration;
	ms_tTextDraws[tID][id].m_bIsActive = true;
	ms_tTextDraws[tID][id].m_nTickUpdate = 1;
}

void eTextDrawProcessor::DisableTextDraw(int id, int tID)
{
	ms_tTextDraws[tID][id].m_bIsActive = false;
	ms_tTextDraws[tID][id].m_nActiveTime = 0;
}

void eTextDrawProcessor::DrawTextDraws(int layerNo)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < TEXTDRAW_NUM; j++)
		{
			eTextDrawEntry& t = ms_tTextDraws[j][i];
			if (t.m_bIsActive && t.m_tConfig.m_nLayerno == layerNo)
			{
				int font = eSystem::GetFont(t.m_tConfig.m_nFont);

				if (font)
				{
					Draw2DText(t.m_szDisplayText, font, &t.m_tParams,
						t.m_tConfig.m_red,
						t.m_tConfig.m_green,
						t.m_tConfig.m_blue,
						t.m_tConfig.m_nAlign);
				}
			}

		}
	}
}

void eTextDrawProcessor::UpdateTextDraws()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < TEXTDRAW_NUM; j++)
		{
			UpdateOneTextDraw(&ms_tTextDraws[j][i]);
		}
	}
}

void eTextDrawProcessor::UpdateOneTextDraw(eTextDrawEntry* ent)
{
	if (!ent->m_bIsActive)
		return;

	if (eSystem::GetTimer() - ent->m_nTimer > ent->m_nTickUpdate) {

		ent->m_nActiveTime--;
		ent->m_nTimer = eSystem::GetTimer();
	}


	if (ent->m_nActiveTime <= 0)
	{
		ent->m_bIsActive = false;
		ent->m_nActiveTime = 0;
	}
}
