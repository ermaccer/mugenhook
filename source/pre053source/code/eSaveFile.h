#pragma once

struct save_header {
	int header; // ERSV
	int size;
	int version;
	int content;
};

struct save_content {
	char type;
	int  row;
	int  column;
	int  charID;
	bool status;
};

class eSaveFile {
public:
	void OpenFile(const char* file);
	void Decrypt();
	void Crypt();
};