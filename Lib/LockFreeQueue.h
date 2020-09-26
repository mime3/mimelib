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

		// 16����Ʈ ����
		alignas(16) volatile LF_Queue_UNIQUE head_;
		alignas(16) volatile LF_Queue_UNIQUE tail_;
		MemoryPool<LF_Queue_NODE> memoryPool_;
		INT64 useSize_ = { 0 };

	public:
		LF_Queue()	// ������
		{
			// ���� ��� ����
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
					// ���� tail�� �������ΰ� ����
					if (next == nullptr)
					{
						// ������ enqueue�Ѵ�
						if (InterlockedCompareExchange64((LONG64*)& tail_.node->next, (LONG64)node, (LONG64)nullptr) == (LONG64)nullptr)
						{
							// tail�� �ű�� �غ���
							InterlockedCompareExchange128((LONG64*)& tail_, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64*)& localTail);
							break;
						}
						// �׻��� ����tail�� tail�̶� �޶�����
						else
						{
							continue;
						}
					}
					// ���� tail�� �������̾ƴϾ���
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
				// Head����
				localHead.unique = head_.unique;
				localHead.node = head_.node;
				// Tail����
				localTail.unique = tail_.unique;
				localTail.node = tail_.node;
				// head�� next����
				next = localHead.node->next;

				//// ���ο� ����
				//if (_useSize == 0)
				//{
				//	if(_head.node == _tail.node)
				//		return false;
				//}

				//// tail �ڿ� node�� ������ �б�
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

				// head�� ���� �ٲ��� �ʾҴ�
				if (localHead.unique == head_.unique)
				{
					// head�� tail�� �پ��ִ�. ����ų� ���� tail�� �Ȱ��ų�
					if (localHead.node == localTail.node)
					{
						// ����head�� next�� ����
						// �� �� ť
						if (next == nullptr)
						{
							return false;
							//// �̸�ŭ �õ��ߴµ� �ȵǸ� ����ġ��
							//if (MAXLOOPCOUNT < loopCount++)
							//{
							//	return false;
							//}
							//// ��� �ٸ������� ���� �غ���
							//Sleep(0);
							//// ������������ �ٽ��� �غ���
							//continue;
						}
						// next�� ������ tail�� ���� �ȿ������� 
						// dequeue�� tail�� ť �����γ�����
						// tail�� next�� �������� ����
						//if (localTail.node->next != nullptr)
						else
						{
							InterlockedCompareExchange128((LONG64*)& tail_, localTail.unique + 1, (LONG64)localTail.node->next, (LONG64*)& localTail);
						}
					}
					// �ϴ� head�� tail�� �������ִ�
					else
					{
						// Ȥ�ø𸣴ϱ�
						if (next == nullptr)
							continue;
						if (localHead.unique != head_.unique)
							continue;
						// ������ dequeue�غ���
						*outData = next->data;
						if (InterlockedCompareExchange128((LONG64*)& head_, localHead.unique + 1, (LONG64)next, (LONG64*)& localHead))
						{
							memoryPool_.Free(localHead.node);
							break;
						}
						// �׻��� head�� �ٲ���� 
						else
						{
							// ó������ ��õ��� �� ���� �Ұž���
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
