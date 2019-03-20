#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <io.h>

#define MAXBLOCK 20

struct Block
{
	char _name[64];
	char *_startPoint, *_endPoint; //���� {,}��ġ
};

class Parser
{
	enum Flag
	{
		LINECOMMENT		= 0x01, // �� �ּ�
		BLOCKCOMMENT	= 0x02, // �����ּ�
		STRINGMODE		= 0x04,	// ���ڿ����
		SPECIALMODE		= 0x08	// Ư��ó�����
	};

	enum BLANKASKII
	{
		SPACE			= 0x20,  // �����̽�
		BACK_SPACE		= 0x08,  // �齺���̽�
		TAB				= 0x09,  // ��
		LINE_FEED		= 0x0a,  // �����ǵ� LF
		CARRIAGE_RETURN = 0x0d   // ĳ�������� CR ,������ (CR LF)
	};

	FILE *_file;
	Block _block[MAXBLOCK];
	int _blockCount;
	int _fileSize;
	unsigned char _flag;
	char *_checkChar[2];
	char _box[2];
	char *_curPointer;  // ���� ���� �����κ��� ù �κ�
	char *_filePointer; // ���� �Ҵ�� �޸�������
	char *_fileEndPointer;

	void Read();
	void UpdateFlag();
	bool GetEqual();
	bool CheckBlank();
	void Jump();
	void CheckBlock();
	bool GetNextWord(char * buffer, char * blockEndPoint);
	char GetFlag();
	void SetFlag(Flag flag);
	void OffFlag(Flag flag);
	bool IsComment();
	bool IsString();
public:
	Parser();
	void PrintAll();
	bool OpenFile(const char * fileName);
	bool CloseFile();


	bool GetValue_BYTE(const char * word, BYTE &outBYTE, const char * blockName);
	bool GetValue_Int(const char * word, int &outInt, const char * blockName);
	bool GetValue_Str(const char * word, char *outString, const char * blockName);


};
