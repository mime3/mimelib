#ifndef __MINLIB_PARSER__
#define __MINLIB_PARSER__
#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <io.h>

#define MAXBLOCK 20
namespace MinLib
{
	struct Block
	{
		char _name[64];
		char* _startPoint, * _endPoint; //각각 {,}위치
		Block()
		{
			ZeroMemory(this, sizeof(Block));
		}
	};

	class Parser
	{
		enum Flag : unsigned char
		{
			LINECOMMENT = 0x01, // 줄 주석
			BLOCKCOMMENT = 0x02, // 블록주석
			STRINGMODE = 0x04,	// 문자열모드
			SPECIALMODE = 0x08	// 특별처리모드
		};

		enum BLANKASKII : CHAR
		{
			SPACE = 0x20,  // 스페이스
			BACK_SPACE = 0x08,  // 백스페이스
			TAB = 0x09,  // 탭
			LINE_FEED = 0x0a,  // 라인피드 LF
			CARRIAGE_RETURN = 0x0d   // 캐리지리턴 CR ,공백은 (CR LF)
		};

		FILE* _file;
		Block _block[MAXBLOCK];
		int _blockCount;
		int _fileSize;
		unsigned char _flag;
		char* _checkChar[2];
		char _box[2];
		char* _curPointer;  // 아직 읽지 않은부분의 첫 부분
		char* _filePointer; // 힙에 할당된 메모리포인터
		char* _fileEndPointer;

		void Read();
		void UpdateFlag();
		bool GetEqual();
		bool CheckBlank();
		bool Jump(char* blockEndPoint = nullptr);
		void CheckBlock();
		bool GetNextWord(char* buffer, char* blockEndPoint);
		unsigned char GetFlag();
		void SetFlag(Flag flag);
		void OffFlag(Flag flag);
		bool IsComment();
		bool IsString();
	public:
		Parser();
		void PrintAll();
		bool OpenFile(const char* fileName);
		bool CloseFile();


		bool GetValue_BYTE(const char* word, BYTE& outBYTE, const char* blockName);
		bool GetValue_Int(const char* word, int& outInt, const char* blockName);
		bool GetValue_Str(const char* word, char* outString, const char* blockName);


	};

	struct BlockUNI
	{
		WCHAR	_name[64];
		WCHAR* _startPoint, * _endPoint; //각각 {,}위치
	};


	class ParserUNI
	{
		enum Flag : unsigned char
		{
			LINECOMMENT = 0x01, // 줄 주석
			BLOCKCOMMENT = 0x02, // 블록주석
			STRINGMODE = 0x04,	// 문자열모드
			SPECIALMODE = 0x08	// 특별처리모드
		};

		enum BLANKUNI : WORD
		{
			//SPACE = 0x20,  // 스페이스
			//BACK_SPACE = 0x08,  // 백스페이스
			//TAB = 0x09,  // 탭
			//LINE_FEED = 0x0a,  // 라인피드 LF
			//CARRIAGE_RETURN = 0x0d   // 캐리지리턴 CR ,공백은 (CR LF)
			SPACE = 0x0020,
			BACK_SPACE = 0x0008,
			TAB = 0x0009,
			LINE_FEED = 0x000A,
			CARRIAGE_RETURN = 0x000D
		};

		FILE* _file;
		BlockUNI _block[MAXBLOCK];
		int _blockCount;
		int _fileSize;
		unsigned char _flag;
		WCHAR* _checkChar[2];
		WCHAR _box[2];
		WCHAR* _curPointer;  // 아직 읽지 않은부분의 첫 부분
		WCHAR* _filePointer; // 힙에 할당된 메모리포인터
		WCHAR* _fileEndPointer;

		void Read();
		void UpdateFlag();
		bool GetEqual();
		bool CheckBlank();
		bool Jump(WCHAR* blockEndPoint = nullptr);
		void CheckBlock();
		bool GetNextWord(WCHAR* buffer, WCHAR* blockEndPoint);
		unsigned char GetFlag();
		void SetFlag(Flag flag);
		void OffFlag(Flag flag);
		bool IsComment();
		bool IsString();
	public:
		ParserUNI();
		void PrintAll();
		bool OpenFile(const WCHAR* fileName);
		bool CloseFile();


		bool GetValue_BYTE(const WCHAR* word, BYTE& outBYTE, const WCHAR* blockName);
		bool GetValue_Int(const WCHAR* word, int& outInt, const WCHAR* blockName);
		bool GetValue_Str(const WCHAR* word, WCHAR* outString, const WCHAR* blockName);

	};
}
#endif // !__MINLIB_PARSER__