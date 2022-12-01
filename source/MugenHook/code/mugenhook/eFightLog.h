#pragma once

#define FIGHTLOG_COMMAND L"-log"


struct eFightLog_Entry {
	int timeLeft;
	int winningSide;
	int life[8];
	int power[8];
};


class eFightLog {
public:
	static void InitialSave();
	static void EndSave();
	static void SaveData();

	static void Clear();
};