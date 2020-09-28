#include "ThreadPool.h"
#include <process.h>

namespace MinLib
{
	ThreadPool::ThreadPool()
	{
		initailize(10);
	}

	ThreadPool::~ThreadPool()
	{
		Stop();
		finalize();
	}

	void ThreadPool::initailize(int threadCount)
	{

	}
	void ThreadPool::finalize()
	{
		CloseHandle(completionPort_);
		CloseHandle(stopEvent_);
	}

	void ThreadPool::InsertQueue(void * data)
	{
		PostQueuedCompletionStatus(completionPort_, 0,  reinterpret_cast<DWORD_PTR>(data), NULL);
	}

	void ThreadPool::Stop()
	{
		int i = 0;

		SetEvent(stopEvent_);

		for (i = 0; i < WORKERTHREAD_COUNT; i++)
		{
			InsertQueue(nullptr);
		}

		WaitForMultipleObjects(WORKERTHREAD_COUNT, &workerThreads_[0], TRUE, INFINITE);
	}

	DWORD ThreadPool::CreateThreads()
	{
		//  Worker thread 积己
		unsigned int threadID;
		int i;

		//  I/O Completion Port 积己
		completionPort_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (completionPort_ == INVALID_HANDLE_VALUE)
			return ERROR_CAN_NOT_COMPLETE;

		for (i = 0; i < WORKERTHREAD_COUNT; i++)
		{
			workerThreads_[i] = (HANDLE)_beginthreadex(0, 0, WorkerThreadMain, this, 0, &threadID);
		}

		stopEvent_ = ::CreateEvent(0, TRUE, FALSE, 0);

		return 0;
	}

	unsigned int ThreadPool::WorkerThreadMain(LPVOID lpData)
	{
		LPOVERLAPPED overlapped = nullptr;
		DWORD numberOfBytes = 0;
		DWORD dwData = 0;
		DWORD * key = nullptr;

		ThreadPool* this_ = static_cast<ThreadPool*>(lpData);

		while (1)
		{
			if (WaitForSingleObject(this_->stopEvent_, 0) == WAIT_OBJECT_0)
				break;

			BOOL isSuccess = GetQueuedCompletionStatus(this_->completionPort_, &numberOfBytes, (PULONG_PTR)&key, &overlapped, INFINITE);
			if (isSuccess && key == nullptr)
				break;

			// Stop Signal
			if (key == nullptr)
				break;

			// used PQCS
			if (overlapped == nullptr)
			{
				// key 狼 Function角青
			}
		}

		return 0;
	}
}
