#pragma once
#include <Windows.h>
#include <stdio.h>
#include <time.h>

//#define PROFILE

#ifdef PROFILE
#define BEGIN(name)		ProFiler::getInstance().BeginProFile(#name)
#define END(name)		ProFiler::getInstance().EndProFile(#name)
#define PRINTPROFILE()	ProFiler::getInstance().FilePrint()
#endif // PROFILE
#ifndef PROFILE
#define BEGIN(name)  
#define END(name)  	
#define PRINTPROFILE()	
#endif // !PROFILE

template <class T>
class SingleTon
{
protected:
	SingleTon() {}
	~SingleTon() {}
public:
	SingleTon(const SingleTon&);

	SingleTon & operator = (const SingleTon &);
	static T  & getInstance()
	{
		static T instance;
		return instance;
	}
};
#define NONCOUNTSIZE 10
#define BLOCKCOUNT 10
#define MAXTHREAD 10

class ProFiler : public SingleTon <ProFiler>
{
	struct ProFileBlock
	{
		bool			_useFlag;
		char			_name[64];
		int				_callCount;
		LARGE_INTEGER	_startTime;
		LONGLONG		_minTime[NONCOUNTSIZE];
		LONGLONG		_maxTime[NONCOUNTSIZE];
		LONGLONG		_totalTime;
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


	FILE *			_file;							// 출력용 파일포인터
	LARGE_INTEGER	_frequency;						// CPU 1초당 클럭
	TLS_BLOCK *		_blockPointerArray[MAXTHREAD];	// 프로파일 블럭포인터를 저장할 배열
	int				_index;							// 빈 배열 인덱스
	DWORD			_tlsIndex;						// TLS인덱스

	// name 이름의 프로파일블록 생성후 포인터를 위치시킨다.
	bool MakeProFile(const char * name, ProFileBlock ** proFilePointer, TLS_BLOCK * blockPointer);
	// name과 같은 프로파일이 이미 있는지 찾아서 포인터를 위치시키고 없으면 MakeProFile을 호출한다.
	bool SearchProFile(const char * name, ProFileBlock ** proFilePointer, TLS_BLOCK * blockPointer);

public:
	ProFiler();
	~ProFiler();
	void BeginProFile(const char * name);
	void EndProFile(const char * name);
	void FilePrint();
	void Reset();
};