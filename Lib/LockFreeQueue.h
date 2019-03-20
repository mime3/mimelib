#pragma once
#include <Windows.h>
#include "MemoryPool.h"

#define MAXLOOPCOUNT 200

template <typename T>
class LF_Queue
{
	struct LF_Queue_NODE
	{
		T data;
		LF_Queue_NODE * next;
	};

	struct LF_Queue_UNIQUE
	{
		LF_Queue_NODE * node;
		INT64			unique;
	};

	alignas(16) volatile LF_Queue_UNIQUE _head;
	alignas(16) volatile LF_Queue_UNIQUE _tail;
	MemoryPool<LF_Queue_NODE> _memoryPool;
	INT64 _useSize;
	
public:
	// 생성자
	LF_Queue()
	{
		// 더미 노드 생성
		_head.node = _memoryPool.Alloc();
		_head.node->next = nullptr;
		_tail.node = _head.node;
		_head.unique = 0;
		_tail.unique = 0;
	}
	// 소멸자
	~LF_Queue()
	{

	}

	void EnQueue(T data)
	{
		LF_Queue_NODE * node = _memoryPool.Alloc();
		node->data = data;
		node->next = nullptr;

		alignas(16) LF_Queue_UNIQUE localTail;

		while (true)
		{
			localTail.unique = _tail.unique;
			localTail.node = _tail.node;

			LF_Queue_NODE * next = localTail.node->next;

			if (localTail.unique == _tail.unique)
			{
				// 로컬 tail이 끝지점인거 같다
				if (next == nullptr)
				{
					// 실제로 enqueue한다
					if (InterlockedCompareExchange64((LONG64 *)&_tail.node->next, (LONG64)node, (LONG64)nullptr) == (LONG64)nullptr)
					{
						// tail을 옮기려 해본다
						InterlockedCompareExchange128((LONG64 *)&_tail, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64 *)&localTail);
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
					InterlockedCompareExchange128((LONG64 *)&_tail, localTail.unique + 1, (LONG64)next, (LONG64 *)&localTail);
				}
			}
		}
		InterlockedIncrement64((LONG64 *)&_useSize);
	}

	bool DeQueue(T * outData)
	{
		if (_useSize == 0)
			return false;
		//__declspec(align(16))
		//__declspec(align(16))
		alignas(16) LF_Queue_UNIQUE localHead;
		alignas(16) LF_Queue_UNIQUE localTail;
		LF_Queue_NODE * next;
		
		int loopCount = 1;
		//while (loopCount++)
		while(1)
		{
			// Head저장
			localHead.unique = _head.unique;
			localHead.node = _head.node;
			// Tail저장
			localTail.unique = _tail.unique;
			localTail.node = _tail.node;
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
			if (localHead.unique == _head.unique)
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
						InterlockedCompareExchange128((LONG64 *)&_tail, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64 *)&localTail);
					}
				}
				// 일단 head와 tail은 떨어져있다
				else
				{
					// 혹시모르니까
					if (next == nullptr)
						continue;
					if (localHead.unique != _head.unique)
						continue;
					// 실제로 dequeue해본다
					*outData = next->data;
					if (InterlockedCompareExchange128((LONG64 *)&_head, localHead.unique + 1, (LONG64)next, (LONG64 *)&localHead))
					{
						_memoryPool.Free(localHead.node);
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
		InterlockedDecrement64((LONG64 *)&_useSize);
		return true;
	}

	bool Peek(T * data, int jumpCount)
	{
		if (jumpCount < 0)
			return false;
		if (GetUseCount() < jumpCount + 1)
			return false;

		LF_Queue_NODE * curNode = _head;

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
		LF_Queue_NODE * node = _memoryPool.Alloc();
		node->data = data;
		node->next = nullptr;

		_tail.node->next = node;
		_tail.node = node;
		_tail.unique++;
		InterlockedIncrement64((LONG64 *)&_useSize);
	}

	bool DeQueue_UnSafe(T * outData)
	{
		if (_useSize == 0)
			return false;

		LF_Queue_NODE * localHead = _head.node;

		_head.node = _head.node->next;
		*outData = _head.node->data;
		_head.unique++;
		_memoryPool.Free(localHead);
		InterlockedDecrement64((LONG64 *)&_useSize);
		return true;
	}

	INT64 GetUseCount()
	{
		return _useSize;
	}
};