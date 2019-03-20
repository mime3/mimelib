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
	char *_startPoint, *_endPoint; //각각 {,}위치
};

class Parser
{
	enum Flag
	{
		LINECOMMENT		= 0x01, // 줄 주석
		BLOCKCOMMENT	= 0x02, // 블록주석
		STRINGMODE		= 0x04,	// 문자열모드
		SPECIALMODE		= 0x08	// 특별처리모드
	};

	enum BLANKASKII
	{
		SPACE			= 0x20,  // 스페이스
		BACK_SPACE		= 0x08,  // 백스페이스
		TAB				= 0x09,  // 탭
		LINE_FEED		= 0x0a,  // 라인피드 LF
		CARRIAGE_RETURN = 0x0d   // 캐리지리턴 CR ,공백은 (CR LF)
	};

	FILE *_file;
	Block _block[MAXBLOCK];
	int _blockCount;
	int _fileSize;
	unsigned char _flag;
	char *_checkChar[2];
	char _box[2];
	char *_curPointer;  // 아직 읽지 않은부분의 첫 부분
	char *_filePointer; // 힙에 할당된 메모리포인터
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

