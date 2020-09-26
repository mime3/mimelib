#ifndef __MINLIB_LOCKFREE_QUEUE__
#define __MINLIB_LOCKFREE_QUEUE__
#pragma once
#include <Windows.h>
#include "MemoryPool.h"

#define MAXLOOPCOUNT 200
namespace MinLib
{
	template <typename T>
	class LF_Queue
	{
		struct LF_Queue_NODE
		{
			T				data;
			LF_Queue_NODE*	next;
		};

		struct LF_Queue_UNIQUE
		{
			LF_Queue_NODE*	node;
			INT64			unique;
		};

		// 16바이트 정렬
		alignas(16) volatile LF_Queue_UNIQUE head_;
		alignas(16) volatile LF_Queue_UNIQUE tail_;
		MemoryPool<LF_Queue_NODE> memoryPool_;
		INT64 useSize_ = { 0 };

	public:
		LF_Queue()	// 생성자
		{
			// 더미 노드 생성
			head_.node = memoryPool_.Alloc();
			head_.node->next = nullptr;
			tail_.node = head_.node;
			head_.unique = 0;
			tail_.unique = 0;
		}
		~LF_Queue() = default;

		void EnQueue(T data)
		{
			LF_Queue_NODE* node = memoryPool_.Alloc();
			node->data = data;
			node->next = nullptr;

			alignas(16) LF_Queue_UNIQUE localTail;

			while (true)
			{
				localTail.unique = tail_.unique;
				localTail.node = tail_.node;

				LF_Queue_NODE* next = localTail.node->next;

				if (localTail.unique == tail_.unique)
				{
					// 로컬 tail이 끝지점인거 같다
					if (next == nullptr)
					{
						// 실제로 enqueue한다
						if (InterlockedCompareExchange64((LONG64*)& tail_.node->next, (LONG64)node, (LONG64)nullptr) == (LONG64)nullptr)
						{
							// tail을 옮기려 해본다
							InterlockedCompareExchange128((LONG64*)& tail_, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64*)& localTail);
							break;
						}
						// 그사이 로컬tail이 tail이랑 달라졌다
						else
						{
							continue;
						}
					}
					// 로컬 tail이 끝지점이아니었다
					else
					{
						InterlockedCompareExchange128((LONG64*)& tail_, localTail.unique + 1, (LONG64)next, (LONG64*)& localTail);
					}
				}
			}
			InterlockedIncrement64((LONG64*)& useSize_);
		}

		bool DeQueue(T* outData)
		{
			if (useSize_ == 0)
				return false;
			//__declspec(align(16))
			//__declspec(align(16))
			alignas(16) LF_Queue_UNIQUE localHead;
			alignas(16) LF_Queue_UNIQUE localTail;
			LF_Queue_NODE* next;

			int loopCount = 1;
			//while (loopCount++)
			while (1)
			{
				// Head저장
				localHead.unique = head_.unique;
				localHead.node = head_.node;
				// Tail저장
				localTail.unique = tail_.unique;
				localTail.node = tail_.node;
				// head의 next저장
				next = localHead.node->next;

				//// 새로운 버전
				//if (_useSize == 0)
				//{
				//	if(_head.node == _tail.node)
				//		return false;
				//}

				//// tail 뒤에 node가 있으면 밀기
				//if (localTail.node->next != nullptr)
				//{
				//	InterlockedCompareExchange128((LONG64 *)&_tail, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64 *)&localTail);
				//}
				//// 
				//else
				//{
				//	if (next != nullptr)
				//	{
				//		*outData = next->data;
				//		if (InterlockedCompareExchange128((LONG64 *)&_head, localHead.unique + 1, (LONG64)localHead.node->next, (LONG64 *)&localHead))
				//		{
				//			_memoryPool.Free(localHead.node);
				//			break;
				//		}
				//	}
				//}

				// head가 아직 바뀌지 않았다
				if (localHead.unique == head_.unique)
				{
					// head와 tail이 붙어있다. 비었거나 아직 tail이 안갔거나
					if (localHead.node == localTail.node)
					{
						// 로컬head에 next가 없다
						// 즉 빈 큐
						if (next == nullptr)
						{
							return false;
							//// 이만큼 시도했는데 안되면 때려치자
							//if (MAXLOOPCOUNT < loopCount++)
							//{
							//	return false;
							//}
							//// 잠깐 다른스레드 먼저 해봐라
							//Sleep(0);
							//// 리턴하지말고 다시좀 해보자
							//continue;
						}
						// next는 있으나 tail이 아직 안움직였다 
						// dequeue시 tail이 큐 밖으로나간다
						// tail의 next가 있을때만 밀자
						//if (localTail.node->next != nullptr)
						else
						{
							InterlockedCompareExchange128((LONG64*)& tail_, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64*)& localTail);
						}
					}
					// 일단 head와 tail은 떨어져있다
					else
					{
						// 혹시모르니까
						if (next == nullptr)
							continue;
						if (localHead.unique != head_.unique)
							continue;
						// 실제로 dequeue해본다
						*outData = next->data;
						if (InterlockedCompareExchange128((LONG64*)& head_, localHead.unique + 1, (LONG64)next, (LONG64*)& localHead))
						{
							memoryPool_.Free(localHead.node);
							break;
						}
						// 그사이 head가 바뀌었다 
						else
						{
							// 처음부터 재시도할 뿐 따로 할거없다
						}
					}
				}
			}
			InterlockedDecrement64((LONG64*)& useSize_);
			return true;
		}

		bool Peek(T* data, int jumpCount)
		{
			if (jumpCount < 0)
				return false;
			if (GetUseCount() < jumpCount + 1)
				return false;

			LF_Queue_NODE* curNode = head_;

			for (int i = 0; i < jumpCount + 1; i++)
			{
				if (curNode->next == nullptr)
					return false;
				curNode = curNode->next;
			}
			*T = curNode->data;
			return true;
		}

		void EnQueue_UnSafe(T data)
		{
			LF_Queue_NODE* node = memoryPool_.Alloc();
			node->data = data;
			node->next = nullptr;

			tail_.node->next = node;
			tail_.node = node;
			tail_.unique++;
			InterlockedIncrement64((LONG64*)& useSize_);
		}

		bool DeQueue_UnSafe(T* outData)
		{
			if (useSize_ == 0)
				return false;

			LF_Queue_NODE* localHead = head_.node;

			head_.node = head_.node->next;
			*outData = head_.node->data;
			head_.unique++;
			memoryPool_.Free(localHead);
			InterlockedDecrement64((LONG64*)& useSize_);
			return true;
		}

		INT64 GetUseCount()
		{
			return useSize_;
		}
	};
}
#endif // !__MINLIB_LOCKFREE_QUEUE__
