﻿#pragma once
//#include "pch.h"
//#include "stdafx.h"
#include <Windows.h>
#include "StreamQueue.h"
namespace MinLib
{
	/*----------------------------------------------------------*/
	// StreamQueue::StreamQueue (public)
	// 설명 : 생성자
	// 인자 : (int) 버퍼 바이트 크기
	// 리턴 : 
	/*----------------*////////////////////////*----------------*/
	StreamQueue::StreamQueue(int iBufferSize)
	{
		Initial(iBufferSize);
	}

	/*----------------------------------------------------------*/
	// StreamQueue::~StreamQueue (public)
	// 설명 : 소멸자
	// 인자 :
	// 리턴 : 
	/*----------------*////////////////////////*----------------*/
	StreamQueue::~StreamQueue()
	{
		delete[] _bufferPointer;
	}

	/*----------------------------------------------------------*/
	// StreamQueue::Initial (protected)
	// 설명 : 초기화
	// 인자 : (int) 버퍼 바이트 크기
	// 리턴 : 
	/*----------------*////////////////////////*----------------*/
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
	/*----------------------------------------------------------*/
	// StreamQueue::Enqueue (public)
	// 설명 : WritePos 에 데이터 Enqueue
	// 인자 : (char *) 데이터 포인터, (int) 크기
	// 리턴 : (int) Enqueue 된 크기.
	/*----------------*////////////////////////*----------------*/
	int StreamQueue::Enqueue(char* chpData, int iSize)
	{
		int freeSize = GetFreeSize();
		if (freeSize < iSize)
			iSize = freeSize;
		if (iSize <= 0)
			return 0;
		char* destPointer = _bufferPointer;
		if (_rear != _availableSize)
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
	/*----------------------------------------------------------*/
	// StreamQueue::Dequeue (public)
	// 설명 : ReadPos 에서 데이터 가져옴. ReadPos 이동
	// 인자 : (char *) 데이터 포인터, (int)크기
	// 리턴 : (int) Dequeue 된 크기.
	/*----------------*////////////////////////*----------------*/
	int StreamQueue::Dequeue(char* chpDest, int iSize)
	{
		int useSize = GetUseSize();
		if (useSize < iSize)
			iSize = useSize;
		char* dataPointer = _bufferPointer;
		if (_front != _availableSize)
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
	/*----------------------------------------------------------*/
	// StreamQueue::Peek (public)
	// 설명 : ReadPos 에서 데이타 읽어옴. ReadPos 고정
	// 인자 : (char *) 데이터 포인터, (int) 크기
	// 리턴 : (int) 가져온 크기
	/*----------------*////////////////////////*----------------*/
	int StreamQueue::Peek(char* chpDest, int iSize, int jumpSize)
	{
		int useSize = GetUseSize();
		useSize -= jumpSize;
		if (useSize < iSize)
			iSize = useSize;
		char* dataPointer = _bufferPointer;
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
}