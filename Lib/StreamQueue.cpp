#pragma once
//#include "pch.h"
//#include "stdafx.h"
#include <Windows.h>
#include "StreamQueue.h"
StreamQueue::StreamQueue(int iBufferSize)
{
	Initial(iBufferSize);
}

StreamQueue::~StreamQueue()
{
	delete[] _bufferPointer;
}

/////////////////////////////////////////////////////////////////////////
// 초기세팅
// Parameters: (int)버퍼 크기
// Return: 
/////////////////////////////////////////////////////////////////////////
void StreamQueue::Initial(int iBufferSize)
{
	_bufferPointer = new char[iBufferSize];
	// front는 항상 빈곳을 가리킨다.
	_front = 0;
	_rear = 0;
	_size = iBufferSize;
	_availableSize = _size - 1;
	_useSize = 0;
}
/////////////////////////////////////////////////////////////////////////
// WritePos 에 데이타 넣음.
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)넣은 크기.
/////////////////////////////////////////////////////////////////////////
int StreamQueue::Enqueue(char * chpData, int iSize)
{
	int freeSize = GetFreeSize();
	if (freeSize < iSize)
		iSize = freeSize;
	if (iSize <= 0)
		return 0;
	char * destPointer = _bufferPointer;
	if(_rear != _availableSize)
		destPointer += _rear + 1;
	int size = GetNotBrokenPutSize();
	if (iSize <= size)
	{
		memcpy_s(destPointer, size, chpData, iSize);
		if (_rear != _availableSize)
			_rear += iSize;
		else
			_rear = iSize - 1;
	}
	else
	{
		memcpy_s(destPointer, size, chpData, size);
		chpData += size;
		memcpy_s(_bufferPointer, iSize - size, chpData, iSize - size);
		_rear = iSize - size - 1;
	}
	//InterlockedAdd((LONG *)&_useSize, iSize);
	//_useSize += iSize;
	return iSize;
}
/////////////////////////////////////////////////////////////////////////
// ReadPos 에서 데이타 가져옴. ReadPos 이동.
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)가져온 크기.
/////////////////////////////////////////////////////////////////////////
int StreamQueue::Dequeue(char * chpDest, int iSize)
{
	int useSize = GetUseSize();
	if (useSize < iSize)
		iSize = useSize;
	char * dataPointer = _bufferPointer;
	if(_front != _availableSize)
		dataPointer += _front + 1;
	int size = GetNotBrokenGetSize();
	if (iSize <= size)
	{
		memcpy_s(chpDest, size, dataPointer, iSize);
		if (_front != _availableSize)
			_front += iSize;
		else
			_front = iSize - 1;
	}
	else
	{
		memcpy_s(chpDest, size, dataPointer, size);
		chpDest += size;
		memcpy_s(chpDest, iSize - size, _bufferPointer, iSize - size);
		_front = iSize - size - 1;
	}
	//InterlockedAdd((LONG *)&_useSize, -iSize);
	//_useSize -= iSize;
	return iSize;
}
/////////////////////////////////////////////////////////////////////////
// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)가져온 크기.
/////////////////////////////////////////////////////////////////////////
int StreamQueue::Peek(char * chpDest, int iSize, int jumpSize)
{
	int useSize = GetUseSize();
	useSize -= jumpSize;
	if (useSize < iSize)
		iSize = useSize;
	char * dataPointer = _bufferPointer;
	if (_front + jumpSize != _availableSize)
		dataPointer += (_front + jumpSize + 1) % _size;
	int size = GetNotBrokenGetSize() - jumpSize;
	if (size <= 0)
	{
		size = useSize;
	}
	if (iSize <= size)
		memcpy_s(chpDest, size, dataPointer, iSize);
	else
	{
		memcpy_s(chpDest, size, dataPointer, size);
		chpDest += size;
		memcpy_s(chpDest, iSize - size, _bufferPointer, iSize - size);
	}
	return iSize;
}
