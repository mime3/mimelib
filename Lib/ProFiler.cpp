//#include "pch.h"
#include "ProFiler.h"
//#include "stdafx.h"

ProFiler::ProFiler()
{
	QueryPerformanceFrequency(&_frequency);
	_tlsIndex = TlsAlloc();
}

ProFiler::~ProFiler()
{
}

bool ProFiler::SearchProFile(const char * name, ProFileBlock ** proFilePointer, TLS_BLOCK * blockPointer)
{
	for (int i = 0; i < BLOCKCOUNT; i++)
	{
		if (strcmp(blockPointer->proFileBlock[i]._name, name) == 0)
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

bool ProFiler::MakeProFile(const char * name, ProFileBlock ** proFilePointer, TLS_BLOCK * blockPointer)
{
	for (int i = 0; i < BLOCKCOUNT; i++)
	{
		if (blockPointer->proFileBlock[i]._useFlag == true)
			continue;

		blockPointer->proFileBlock[i]._useFlag = true;
		strcpy_s(blockPointer->proFileBlock[i]._name, name); //strcpy는 dest의 버퍼가 작을경우에 주의한다.
		for (auto & minTime : blockPointer->proFileBlock[i]._minTime)
		{
			minTime = 0x7FFFFFFFFFFFFFFF;
		}
		*proFilePointer = &blockPointer->proFileBlock[i];
		return true;
	}
	return false;

	//for (auto & proFile : _proFileBlock)
	//{
	//	if (proFile._useFlag == true)
	//		continue;

	//	proFile._useFlag = true;
	//	strcpy_s(proFile._name, name); //strcpy는 dest의 버퍼가 작을경우에 주의한다.
	//	for (auto & minTime : proFile._minTime)
	//	{
	//		minTime = 0x7FFFFFFFFFFFFFFF;
	//	}
	//	*proFilePointer = &proFile;
	//	return true;
	//}
	//return false;
}

void ProFiler::BeginProFile(const char * name)
{
	TLS_BLOCK * blockPointer = (TLS_BLOCK *)TlsGetValue(_tlsIndex);
	if (blockPointer == nullptr)
	{
		blockPointer = new TLS_BLOCK;
		TlsSetValue(_tlsIndex, blockPointer);
		blockPointer->threadID = GetCurrentThreadId();
		int index = InterlockedIncrement((LONG *)&_index) - 1;
		_blockPointerArray[index] = blockPointer;
	}
	ProFileBlock * proFilePointer = nullptr;

	if (!SearchProFile(name, &proFilePointer, blockPointer))
		if (!MakeProFile(name, &proFilePointer, blockPointer))
			throw;

	if (proFilePointer->_startTime.QuadPart != 0)
		throw;

	QueryPerformanceCounter(&proFilePointer->_startTime);
	proFilePointer->_callCount++;
}

void ProFiler::EndProFile(const char * name)
{
	TLS_BLOCK * blockPointer = (TLS_BLOCK *)TlsGetValue(_tlsIndex);
	if (blockPointer == nullptr)
		throw;

	ProFileBlock * proFilePointer = nullptr;
	if (!SearchProFile(name, &proFilePointer, blockPointer))
		throw;

	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);
	LONGLONG time = endTime.QuadPart - proFilePointer->_startTime.QuadPart;
	proFilePointer->_totalTime += time;
	proFilePointer->_startTime.QuadPart = 0;

	// 작은거 밀어내면서 저장
	for (int i = 0; i < sizeof(proFilePointer->_minTime) / sizeof(LONGLONG); ++i)
	{
		if (time < proFilePointer->_minTime[i])
		{
			for (int j = sizeof(proFilePointer->_minTime) / sizeof(LONGLONG) - 1; j != i; --j)
			{
				proFilePointer->_minTime[j] = proFilePointer->_minTime[j - 1];
			}
			proFilePointer->_minTime[i] = time;
			break;
		}
	}
	// 큰거 밀어내면서 저장
	for (int i = 0; i < sizeof(proFilePointer->_maxTime) / sizeof(LONGLONG); ++i)
	{
		if (proFilePointer->_maxTime[i] < time)
		{
			for (int j = sizeof(proFilePointer->_maxTime) / sizeof(LONGLONG) - 1; j != i; --j)
			{
				proFilePointer->_maxTime[j] = proFilePointer->_maxTime[j-1];
			}
			proFilePointer->_maxTime[i] = time;
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
	fopen_s(&_file, fileName, "wt");

	//char buffer[128];
	//sprintf_s(buffer, "%20s | %8s | %8s | %8s | %8s\n", "Name", "Average", "Min", "Max", "Call");
	//fwrite(buffer, sizeof(buffer), 1, _file);
	fprintf_s(_file, "%10s | %15s | %15s | %15s | %15s | %15s\n", "ThreadID", "Name", "Average", "Min", "Max", "Call");
	for (int block = 0; block < MAXTHREAD; block++)
	{
		if (_blockPointerArray[block] == nullptr)
			break;

		for (auto & ProFileBlock : _blockPointerArray[block]->proFileBlock)
		{
			if (ProFileBlock._useFlag == false)
				continue;

			LONGLONG minusForAverage = 0;
			int passed = 0;
			for (int i = 0; i < sizeof(ProFileBlock._minTime) / sizeof(LONGLONG) - 1; ++i)
			{
				if (ProFileBlock._minTime[i] == 0x7FFFFFFFFFFFFFFF)
				{
					passed++;
					continue;
				}
				minusForAverage += ProFileBlock._minTime[i];
			}
			for (int i = 0; i < sizeof(ProFileBlock._maxTime) / sizeof(LONGLONG) - 1; ++i)
			{
				if (ProFileBlock._maxTime[i] == 0)
				{
					passed++;
					continue;
				}
				minusForAverage += ProFileBlock._maxTime[i];
			}
			// 총시간 / 카운트 / 진동수
			long double average;
			long double minTime;
			long double maxTime;
			// 충분한 호출이 이루어지지 않았다. (버리는양이 더 많다)
			if (ProFileBlock._totalTime < minusForAverage)
			{
				average = 
					(long double)(ProFileBlock._totalTime)
					/ _frequency.QuadPart
					/ ProFileBlock._callCount;
				minTime = 
					(long double)ProFileBlock._minTime[0] 
					/ _frequency.QuadPart;
				maxTime = (
					long double)ProFileBlock._maxTime[0] 
					/ _frequency.QuadPart;
			}
			else
			{
				average = 
					(long double)(ProFileBlock._totalTime - minusForAverage) 
					/ _frequency.QuadPart
					/ ((long double)(ProFileBlock._callCount - (sizeof(ProFileBlock._minTime) / sizeof(LONGLONG) - sizeof(ProFileBlock._maxTime) / sizeof(LONGLONG) - 2 - passed)));
				minTime =
					(long double)ProFileBlock._minTime[sizeof(ProFileBlock._maxTime) / sizeof(LONGLONG) - 1]
					/ _frequency.QuadPart;
				maxTime =
					(long double)ProFileBlock._maxTime[sizeof(ProFileBlock._maxTime) / sizeof(LONGLONG) - 1]
					/ _frequency.QuadPart;
			}
			average *= 1000000;
			minTime *= 1000000;
			maxTime *= 1000000;

		
			fprintf_s(_file, "%10d | %15s | %13.4lf㎲ | %13.4lf㎲ | %13.4lf㎲ | %15ld\n",
				_blockPointerArray[block]->threadID, ProFileBlock._name, average, minTime, maxTime, ProFileBlock._callCount);
		}
		fprintf_s(_file, "----------------------------------------------------------------------------------------------------\n");
	}
	fclose(_file);
}

void ProFiler::Reset()
{
	for (int i = 0; i < MAXTHREAD; i++)
	{
		for (auto & ProFileBlock : _blockPointerArray[i]->proFileBlock)
		{
			ProFileBlock._useFlag = false;
			ProFileBlock._callCount = 0;
			ProFileBlock._totalTime = 0;
		}
	}
}

