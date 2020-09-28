#ifndef __MINLIB_SIGLETON_PROFILE__
#define __MINLIB_SIGLETON_PROFILE__
#pragma once
#include "SingleTon.h"

#include <Windows.h>
#include <stdio.h>
#include <time.h>

//#define PROFILE
namespace MinLib
{
#ifdef PROFILE
#define AUTOPROFILE(name)	MinLib::AutoProFile(name)
#define BEGIN(name)			MinLib::ProFiler::getInstance().BeginProFile(name)
#define END(name)			MinLib::ProFiler::getInstance().EndProFile(name)
#define PRINTPROFILE()		MinLib::ProFiler::getInstance().FilePrint()
#endif // PROFILE
#ifndef PROFILE
#define	AUTOPROFILE(name)
#define BEGIN(name)  
#define END(name)  	
#define PRINTPROFILE()	
#endif // !PROFILE

#define NONCOUNTSIZE 10
#define BLOCKCOUNT 10
#define MAXTHREAD 10

	class ProFiler : public SingleTon <ProFiler>
	{
		struct ProFileBlock
		{
			bool			useFlag;
			char			name[64];
			int				callCount;
			LARGE_INTEGER	startTime;
			LONGLONG		minTime[NONCOUNTSIZE];
			LONGLONG		maxTime[NONCOUNTSIZE];
			LONGLONG		totalTime;
		};

		struct TLS_BLOCK
		{
			ProFileBlock proFileBlock[BLOCKCOUNT];
			DWORD threadID;
			TLS_BLOCK()
			{
				ZeroMemory(&proFileBlock, sizeof(ProFileBlock) * BLOCKCOUNT);
				threadID = NULL;
			}
		};


		FILE*			file_;							// 출력용 파일포인터
		LARGE_INTEGER	frequency_;						// CPU 1초당 클럭
		TLS_BLOCK*		blockPointerArray_[MAXTHREAD];	// 프로파일 블럭포인터를 저장할 배열
		int				index_;							// 빈 배열 인덱스
		DWORD			tlsIndex_;						// TLS인덱스

		// name 이름의 프로파일블록 생성후 포인터를 위치시킨다.
		bool MakeProFile(const char* name, ProFileBlock** proFilePointer, TLS_BLOCK* blockPointer);
		// name과 같은 프로파일이 이미 있는지 찾아서 포인터를 위치시키고 없으면 MakeProFile을 호출한다.
		bool SearchProFile(const char* name, ProFileBlock** proFilePointer, TLS_BLOCK* blockPointer);

	public:
		ProFiler();
		~ProFiler() = default;
		void BeginProFile(const char* name);
		void EndProFile(const char* name);
		void FilePrint();
		void Reset();
	};

	class AutoProFile
	{
	public:
		AutoProFile(const char* name);
		~AutoProFile();

	private:
		char name_[64];
	};
}
#endif // !__MINLIB_SIGLETON_PROFILE__