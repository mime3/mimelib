//#include "pch.h"
#include "ProFiler.h"
//#include "stdafx.h"

namespace MinLib
{
	ProFiler::ProFiler()
	{
		QueryPerformanceFrequency(&frequency_);
		tlsIndex_ = TlsAlloc();
	}

	bool ProFiler::SearchProFile(const char* name, ProFileBlock** proFilePointer, TLS_BLOCK* blockPointer)
	{
		for (int i = 0; i < BLOCKCOUNT; i++)
		{
			if (strcmp(blockPointer->proFileBlock[i].name, name) == 0)
			{
				*proFilePointer = &blockPointer->proFileBlock[i];
				return true;
			}
		}
		return false;
		//for (auto & proFile : _proFileBlock)
		//{
		//	if (strcmp(proFile._name, name) == 0)
		//	{
		//		*proFilePointer = &proFile;
		//		return true;
		//	}
		//}
		//return false;
	}

	bool ProFiler::MakeProFile(const char* name, ProFileBlock** proFilePointer, TLS_BLOCK* blockPointer)
	{
		for (int i = 0; i < BLOCKCOUNT; i++)
		{
			if (blockPointer->proFileBlock[i].useFlag == true)
				continue;

			blockPointer->proFileBlock[i].useFlag = true;
			strcpy_s(blockPointer->proFileBlock[i].name, name); //strcpy는 dest의 버퍼가 작을경우에 주의한다.
			for (auto& minTime : blockPointer->proFileBlock[i].minTime)
			{
				minTime = 0x7FFFFFFFFFFFFFFF;
			}
			*proFilePointer = &blockPointer->proFileBlock[i];
			return true;
		}
		return false;

		//for (auto & proFile : _proFileBlock)
		//{
		//	if (proFile.useFlag == true)
		//		continue;

		//	proFile.useFlag = true;
		//	strcpy_s(proFile._name, name); //strcpy는 dest의 버퍼가 작을경우에 주의한다.
		//	for (auto & minTime : proFile.minTime)
		//	{
		//		minTime = 0x7FFFFFFFFFFFFFFF;
		//	}
		//	*proFilePointer = &proFile;
		//	return true;
		//}
		//return false;
	}

	void ProFiler::BeginProFile(const char* name)
	{
		TLS_BLOCK* blockPointer = (TLS_BLOCK*)TlsGetValue(tlsIndex_);
		if (blockPointer == nullptr)
		{
			blockPointer = new TLS_BLOCK;
			TlsSetValue(tlsIndex_, blockPointer);
			blockPointer->threadID = GetCurrentThreadId();
			int index = InterlockedIncrement((LONG*)&index_) - 1;
			blockPointerArray_[index] = blockPointer;
		}
		ProFileBlock* proFilePointer = nullptr;

		if (!SearchProFile(name, &proFilePointer, blockPointer))
			if (!MakeProFile(name, &proFilePointer, blockPointer))
				throw;

		if (proFilePointer->startTime.QuadPart != 0)
			throw;

		QueryPerformanceCounter(&proFilePointer->startTime);
		proFilePointer->callCount++;
	}

	void ProFiler::EndProFile(const char* name)
	{
		TLS_BLOCK* blockPointer = (TLS_BLOCK*)TlsGetValue(tlsIndex_);
		if (blockPointer == nullptr)
			throw;

		ProFileBlock* proFilePointer = nullptr;
		if (!SearchProFile(name, &proFilePointer, blockPointer))
			throw;

		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);
		LONGLONG time = endTime.QuadPart - proFilePointer->startTime.QuadPart;
		proFilePointer->totalTime += time;
		proFilePointer->startTime.QuadPart = 0;

		// 작은거 밀어내면서 저장
		for (int i = 0; i < sizeof(proFilePointer->minTime) / sizeof(LONGLONG); ++i)
		{
			if (time < proFilePointer->minTime[i])
			{
				for (int j = sizeof(proFilePointer->minTime) / sizeof(LONGLONG) - 1; j != i; --j)
				{
					proFilePointer->minTime[j] = proFilePointer->minTime[j - 1];
				}
				proFilePointer->minTime[i] = time;
				break;
			}
		}
		// 큰거 밀어내면서 저장
		for (int i = 0; i < sizeof(proFilePointer->maxTime) / sizeof(LONGLONG); ++i)
		{
			if (proFilePointer->maxTime[i] < time)
			{
				for (int j = sizeof(proFilePointer->maxTime) / sizeof(LONGLONG) - 1; j != i; --j)
				{
					proFilePointer->maxTime[j] = proFilePointer->maxTime[j - 1];
				}
				proFilePointer->maxTime[i] = time;
				break;
			}
		}
	}

	void ProFiler::FilePrint()
	{
		char fileName[39];
		tm _tm;
		time_t _time = time(NULL);
		localtime_s(&_tm, &_time);
		sprintf_s(fileName, "ProFiler_%04d%02d%02d_%02d%02d%02d.txt",
			_tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
		fopen_s(&file_, fileName, "wt");

		//char buffer[128];
		//sprintf_s(buffer, "%20s | %8s | %8s | %8s | %8s\n", "Name", "Average", "Min", "Max", "Call");
		//fwrite(buffer, sizeof(buffer), 1, _file);
		fprintf_s(file_, "%10s | %15s | %15s | %15s | %15s | %15s\n", "ThreadID", "Name", "Average", "Min", "Max", "Call");
		for (int block = 0; block < MAXTHREAD; block++)
		{
			if (blockPointerArray_[block] == nullptr)
				break;

			for (auto& ProFileBlock : blockPointerArray_[block]->proFileBlock)
			{
				if (ProFileBlock.useFlag == false)
					continue;

				LONGLONG minusForAverage = 0;
				int passed = 0;
				for (int i = 0; i < sizeof(ProFileBlock.minTime) / sizeof(LONGLONG) - 1; ++i)
				{
					if (ProFileBlock.minTime[i] == 0x7FFFFFFFFFFFFFFF)
					{
						passed++;
						continue;
					}
					minusForAverage += ProFileBlock.minTime[i];
				}
				for (int i = 0; i < sizeof(ProFileBlock.maxTime) / sizeof(LONGLONG) - 1; ++i)
				{
					if (ProFileBlock.maxTime[i] == 0)
					{
						passed++;
						continue;
					}
					minusForAverage += ProFileBlock.maxTime[i];
				}
				// 총시간 / 카운트 / 진동수
				long double average;
				long double minTime;
				long double maxTime;
				// 충분한 호출이 이루어지지 않았다. (버리는양이 더 많다)
				if (ProFileBlock.totalTime < minusForAverage)
				{
					average =
						(long double)(ProFileBlock.totalTime)
						/ frequency_.QuadPart
						/ ProFileBlock.callCount;
					minTime =
						(long double)ProFileBlock.minTime[0]
						/ frequency_.QuadPart;
					maxTime = (
						long double)ProFileBlock.maxTime[0]
						/ frequency_.QuadPart;
				}
				else
				{
					average =
						(long double)(ProFileBlock.totalTime - minusForAverage)
						/ frequency_.QuadPart
						/ ((long double)(ProFileBlock.callCount - (sizeof(ProFileBlock.minTime) / sizeof(LONGLONG) - sizeof(ProFileBlock.maxTime) / sizeof(LONGLONG) - 2 - passed)));
					minTime =
						(long double)ProFileBlock.minTime[sizeof(ProFileBlock.maxTime) / sizeof(LONGLONG) - 1]
						/ frequency_.QuadPart;
					maxTime =
						(long double)ProFileBlock.maxTime[sizeof(ProFileBlock.maxTime) / sizeof(LONGLONG) - 1]
						/ frequency_.QuadPart;
				}
				average *= 1000000;
				minTime *= 1000000;
				maxTime *= 1000000;


				fprintf_s(file_, "%10d | %15s | %13.4lf㎲ | %13.4lf㎲ | %13.4lf㎲ | %15ld\n",
					blockPointerArray_[block]->threadID, ProFileBlock.name, average, minTime, maxTime, ProFileBlock.callCount);
			}
			fprintf_s(file_, "----------------------------------------------------------------------------------------------------\n");
		}
		fclose(file_);
	}

	void ProFiler::Reset()
	{
		for (int i = 0; i < MAXTHREAD; i++)
		{
			for (auto& ProFileBlock : blockPointerArray_[i]->proFileBlock)
			{
				ProFileBlock.useFlag = false;
				ProFileBlock.callCount = 0;
				ProFileBlock.totalTime = 0;
			}
		}
	}

	AutoProFile::AutoProFile(const char * name)
	{
		BEGIN(name);
		strcpy_s(name_, name);
	}

	AutoProFile::~AutoProFile()
	{
		END(name_);
	}

}