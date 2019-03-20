#pragma once
#include "pch.h"
//#include "stdafx.h"
#include "StreamBuffer.h"

/*----------------------------------------------------------*/
// StreamBuffer::StreamBuffer (public)
// 설명 : 생성자
// 인자 : (int) 버퍼 사이즈
// 리턴 :
/*----------------*////////////////////////*----------------*/
StreamBuffer::StreamBuffer(int size)
{
	_buffer = new char[size];
	_front = 0;
	_rear = 0;
	_size = size;
	_refCount = 1;
	_headerFillFlag = false;
}

/*----------------------------------------------------------*/
// StreamBuffer::~StreamBuffer (public)
// 설명 : 소멸자, 버퍼를 해제한다
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
StreamBuffer::~StreamBuffer()
{
	delete[] _buffer;
}





/*----------------------------------------------------------*/
// StreamBuffer::Alloc (public)
// 설명 : 직렬화 버퍼를 메모리 풀에서 할당
// 인자 : (int) size <미사용>
// 리턴 : (StreamBuffer *) 직렬화 버퍼 포인터
/*----------------*////////////////////////*----------------*/
StreamBuffer * StreamBuffer::Alloc(int headerSize)
{
	StreamBuffer * newBuffer = _memoryPool.Alloc();
	newBuffer->Clear();
	newBuffer->_rear += headerSize;
	InterlockedIncrement((LONG *)&_allocCount);
	return newBuffer;
}

/*----------------------------------------------------------*/
// StreamBuffer::Free (public)
// 설명 : 직렬화 버퍼를 메모리 풀에 반환
// 인자 : (StreamBuffer *) 직렬화 버퍼 포인터
// 리턴 :
/*----------------*////////////////////////*----------------*/
void StreamBuffer::Free(StreamBuffer * buffer)
{
	int count = InterlockedDecrement((LONG *)&buffer->_refCount);
	if (count == 0)
	{
		InterlockedDecrement((LONG *)&_allocCount);
		_memoryPool.Free(buffer);
	}
}

int StreamBuffer::_allocCount = 0;
MemoryPoolTLS<StreamBuffer> StreamBuffer::_memoryPool;