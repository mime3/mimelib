#pragma once
//#include "pch.h"
//#include "stdafx.h"
#include "StreamBuffer.h"
namespace MinLib
{
	/*----------------------------------------------------------*/
	// StreamBuffer::StreamBuffer (public)
	// 설명 : 생성자
	// 인자 : (int) 버퍼 사이즈
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	StreamBuffer::StreamBuffer(int size)
	{
		buffer_ = new char[size];
		front_ = 0;
		rear_ = 0;
		size_ = size;
		refCount_ = 1;
		headerFillFlag_ = false;
	}

	/*----------------------------------------------------------*/
	// StreamBuffer::~StreamBuffer (public)
	// 설명 : 소멸자, 버퍼를 해제한다
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	StreamBuffer::~StreamBuffer()
	{
		delete[] buffer_;
	}

	/*----------------------------------------------------------*/
	// StreamBuffer::Alloc (public)
	// 설명 : 직렬화 버퍼를 메모리 풀에서 할당
	// 인자 : (int) header 영역 size 
	// 리턴 : (StreamBuffer *) 직렬화 버퍼 포인터
	/*----------------*////////////////////////*----------------*/
	StreamBuffer* StreamBuffer::Alloc(int headerSize)
	{
		StreamBuffer* newBuffer = memoryPool_.Alloc();
		newBuffer->Clear();
		newBuffer->rear_ += headerSize;
		InterlockedIncrement((LONG*)&allocCount_);
		return newBuffer;
	}

	/*----------------------------------------------------------*/
	// StreamBuffer::Free (public)
	// 설명 : 직렬화 버퍼를 메모리 풀에 반환
	// 인자 : (StreamBuffer *) 직렬화 버퍼 포인터
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	void StreamBuffer::Free(StreamBuffer* buffer)
	{
		int count = InterlockedDecrement((LONG*)&buffer->refCount_);
		if (count == 0)
		{
			InterlockedDecrement((LONG*)&allocCount_);
			memoryPool_.Free(buffer);
		}
	}

	int StreamBuffer::allocCount_ = 0;
	MemoryPoolTLS<StreamBuffer> StreamBuffer::memoryPool_;
}