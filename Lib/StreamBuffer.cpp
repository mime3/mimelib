#pragma once
//#include "pch.h"
//#include "stdafx.h"
#include "StreamBuffer.h"
namespace MinLib
{
	/*----------------------------------------------------------*/
	// StreamBuffer::StreamBuffer (public)
	// ���� : ������
	// ���� : (int) ���� ������
	// ���� :
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
	// ���� : �Ҹ���, ���۸� �����Ѵ�
	// ���� : 
	// ���� :
	/*----------------*////////////////////////*----------------*/
	StreamBuffer::~StreamBuffer()
	{
		delete[] buffer_;
	}

	/*----------------------------------------------------------*/
	// StreamBuffer::Alloc (public)
	// ���� : ����ȭ ���۸� �޸� Ǯ���� �Ҵ�
	// ���� : (int) header ���� size 
	// ���� : (StreamBuffer *) ����ȭ ���� ������
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
	// ���� : ����ȭ ���۸� �޸� Ǯ�� ��ȯ
	// ���� : (StreamBuffer *) ����ȭ ���� ������
	// ���� :
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