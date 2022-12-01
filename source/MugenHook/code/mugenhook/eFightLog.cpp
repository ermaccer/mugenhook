#include "eFightLog.h"
#include "base.h"
#include "..\mugen\System.h"
#include <iostream>
#include "eCommandLineParams.h"
#include "..\mugen\Data.h"

void eFightLog::InitialSave()
{
	SaveData();
	Call<0x424310>();
}

void eFightLog::EndSave()
{
	SaveData();
}

void eFightLog::SaveData()
{
	if (!CommandLine::ArgumentExists(FIGHTLOG_COMMAND))
		return;

	eMugenData* data = eSystem::GetData();
	int roundNo = data->GetRound();

	if (char* path = CommandLine::GetStringArgument(FIGHTLOG_COMMAND))
	{
		FILE* log = fopen(path, "a+");
		if (log)
		{
			int totalMatches = data->GetTotalMatches();
			int winner = data->GetWinningSide();
			int time = eSystem::GetRoundTime();

			fprintf(log, "[Match %d Round %d]\n", totalMatches, roundNo);
			fprintf(log, "winningteam = %d\n", winner);
			fprintf(log, "timeleft = %.2f\n", time / 60.0f);
			fprintf(log, "stage = %s\n", data->LastStage);
			
			// print teams

			int amount_p1 = data->GetTeamCharactersAmount(TEAM_1);

			if (amount_p1 >= 0)
			{
				for (int i = 0; i < amount_p1; i++)
				{
					int id = 1 + (i * 2);
					if (eMugenCharacter* chr = data->GetCharacter(id))
					{
						fprintf(log, "p%d.displayName = %s\n", id, chr->Info->RealName);
						fprintf(log, "p%d.name = %s\n", id, chr->ScriptName);
						fprintf(log, "p%d.life = %d\n", id, chr->Health);
						fprintf(log, "p%d.power = %d\n", id, chr->Power);
					}
				}
			}

			int amount_p2 = data->GetTeamCharactersAmount(TEAM_2);
			if (amount_p2 >= 0)
			{
				for (int i = 0; i < amount_p2; i++)
				{
					int id = 2 + (i * 2);
					if (eMugenCharacter* chr = data->GetCharacter(id))
					{
						fprintf(log, "p%d.displayName = %s\n", id, chr->Info->RealName);
						fprintf(log, "p%d.name = %s\n", id, chr->ScriptName);
						fprintf(log, "p%d.life = %d\n", id, chr->Health);
						fprintf(log, "p%d.power = %d\n", id, chr->Power);
					}
				}
			}
			fprintf(log, "\n");
			fclose(log);
		}
	}
}

void eFightLog::Clear()
{
	if (!CommandLine::ArgumentExists(FIGHTLOG_COMMAND))
		return;

	if (char* path = CommandLine::GetStringArgument(FIGHTLOG_COMMAND))
	{
		FILE* log = fopen(path, "w");
		if (log)
		{
			fclose(log);
		}
	}
}
