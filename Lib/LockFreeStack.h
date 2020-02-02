#ifndef __MINLIB_LOCKFREE_STACK__
#define __MINLIB_LOCKFREE_STACK__


#pragma once
#include <windows.h>
#include "MemoryPool.h"
namespace MinLib
{
	template <typename T>
	class LF_Stack
	{
		struct BLOCK
		{
			BLOCK*	next = { nullptr };
			T		data = {};
		};

		struct BYTE16TOP
		{
			BLOCK*	topBlock = { nullptr };
			INT64	unique = { 0 };
		};

		alignas(16) BYTE16TOP	topBlock_	= {};
		BLOCK*					nullBlock_	= { nullptr };
		INT64					useCount_	= { 0 };
		MemoryPool<BLOCK>		memoryPool_ = {};

	public:
		LF_Stack()
		{
			nullBlock_ = new BLOCK();
			ZeroMemory(&nullBlock_->data, sizeof(T));
			topBlock_.topBlock = nullBlock_;
			topBlock_.unique = 0;
			useCount_ = 0;
		}
		void Push(T data)
		{
			alignas(16) BYTE16TOP saveTop;
			BLOCK* newBlock = memoryPool_.Alloc();
			newBlock->data = data;
			do
			{
				saveTop.topBlock = topBlock_.topBlock;
				saveTop.unique = topBlock_.unique;
				newBlock->next = saveTop.topBlock;
				//} while (!InterlockedCompareExchange64((LONG64 *)&_topBlock._topBlock, (LONG64)newBlock, (LONG64)saveTop[0]));
			} while (!InterlockedCompareExchange128((LONG64*)& topBlock_.topBlock, (LONG64)saveTop.unique + 1, (LONG64)newBlock, (LONG64*)& saveTop));
			InterlockedIncrement64((LONG64*)& useCount_);
		}
		bool Pop(T* outData)
		{
			alignas(16) BYTE16TOP saveTop;
			BLOCK* newTop;
			do
			{
				saveTop.topBlock = topBlock_.topBlock;
				if (saveTop.topBlock == nullBlock_)
					return false;
				saveTop.unique = topBlock_.unique;
				newTop = saveTop.topBlock->next;
			} while (!InterlockedCompareExchange128((LONG64*)& topBlock_.topBlock, (LONG64)saveTop.unique + 1, (LONG64)newTop, (LONG64*)& saveTop));
			*outData = saveTop.topBlock->data;
			InterlockedDecrement64((LONG64*)& useCount_);
			memoryPool_.Free(saveTop.topBlock);
			return true;
		}

		INT64 GetUseCount()
		{
			return useCount_;
		}
	};
}
#endif // !__MINLIB_LOCKFREE_STACK__
