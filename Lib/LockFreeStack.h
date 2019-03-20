#pragma once
#include <windows.h>
#include "MemoryPool.h"

template <typename T>
class LF_Stack
{
	struct BLOCK
	{
		BLOCK * next;
		T data;
		BLOCK() 
		{
			next = nullptr;
		}
	};

	struct BYTE16TOP
	{
		BLOCK * _topBlock;
		INT64	_unique;
	};

	alignas(16) BYTE16TOP _topBlock;
	BLOCK * _nullBlock;
	INT64	_useCount;
	MemoryPool<BLOCK> _memoryPool;

public:
	LF_Stack()
	{
		_nullBlock = new BLOCK();
		ZeroMemory(&_nullBlock->data, sizeof(T));
		_topBlock._topBlock = _nullBlock;
		_topBlock._unique = 0;
		_useCount = 0;
	}
	void Push(T data)
	{
		alignas(16) BYTE16TOP saveTop;
		BLOCK * newBlock = _memoryPool.Alloc();
		newBlock->data = data;
		do
		{
			saveTop._topBlock = _topBlock._topBlock;
			saveTop._unique = _topBlock._unique;
			newBlock->next = saveTop._topBlock;
		//} while (!InterlockedCompareExchange64((LONG64 *)&_topBlock._topBlock, (LONG64)newBlock, (LONG64)saveTop[0]));
		} while (!InterlockedCompareExchange128((LONG64 *)&_topBlock._topBlock, (LONG64)saveTop._unique + 1, (LONG64)newBlock, (LONG64 *)&saveTop));
		InterlockedIncrement64((LONG64 *)&_useCount);
	}
	bool Pop(T * outData)
	{
		alignas(16) BYTE16TOP saveTop;
		BLOCK * newTop;
		do
		{
			saveTop._topBlock = _topBlock._topBlock;
			if (saveTop._topBlock == _nullBlock)
				return false;
			saveTop._unique = _topBlock._unique;
			newTop = saveTop._topBlock->next;
		} while (!InterlockedCompareExchange128((LONG64 *)&_topBlock._topBlock, (LONG64)saveTop._unique + 1, (LONG64)newTop, (LONG64 *)&saveTop));
		*outData = saveTop._topBlock->data;
		InterlockedDecrement64((LONG64 *)&_useCount);
		_memoryPool.Free(saveTop._topBlock);
		return true;
	}

	INT64 GetUseCount()
	{
		return _useCount;
	}
};