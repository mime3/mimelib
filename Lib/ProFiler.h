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


		FILE*			file_;							// ��¿� ����������
		LARGE_INTEGER	frequency_;						// CPU 1�ʴ� Ŭ��
		TLS_BLOCK*		blockPointerArray_[MAXTHREAD];	// �������� �������͸� ������ �迭
		int				index_;							// �� �迭 �ε���
		DWORD			tlsIndex_;						// TLS�ε���

		// name �̸��� �������Ϻ�� ������ �����͸� ��ġ��Ų��.
		bool MakeProFile(const char* name, ProFileBlock** proFilePointer, TLS_BLOCK* blockPointer);
		// name�� ���� ���������� �̹� �ִ��� ã�Ƽ� �����͸� ��ġ��Ű�� ������ MakeProFile�� ȣ���Ѵ�.
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