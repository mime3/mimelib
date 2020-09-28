#ifndef __MINLIB_THREAD_POOL__
#define	__MINLIB_THREAD_POOL__
#pragma once
#include "SingleTon.h"

#include <array>
#include <windows.h>

#define WORKERTHREAD_COUNT 10

namespace MinLib
{
	///////////////////////////////////////////////////
	// IOCP 스레드 풀
	///////////////////////////////////////////////////
	class ThreadPool : public SingleTon<ThreadPool>
	{
	public:
		ThreadPool();
		~ThreadPool();

		void initailize(int threadCount = 0);
		void finalize();

		void InsertQueue(void * data);
		void Stop();

	private:
		DWORD CreateThreads();
		static unsigned int __stdcall WorkerThreadMain(LPVOID lpData);

	private:
		HANDLE completionPort_		= { NULL };
		HANDLE stopEvent_			= { NULL };
		std::array<HANDLE, WORKERTHREAD_COUNT> workerThreads_ = { NULL };

	};
}

#endif // !__MINLIB_THREAD_POOL__