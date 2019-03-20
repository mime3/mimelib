#include "pch.h"
#include "CpuUsage.h"

CpuUsage::CpuUsage(HANDLE process)
{
	// 핸들이 없다면 자기자신을 대상
	if (process == INVALID_HANDLE_VALUE)
	{
		_process = GetCurrentProcess();
	}

	// 프로세서 개수 확인
	SYSTEM_INFO systemInfo;

	GetSystemInfo(&systemInfo);
	_numberOfProcessors = systemInfo.dwNumberOfProcessors;

	_processorTotal = 0;
	_processorUser = 0;
	_processorKernel = 0;

	_processTotal = 0;
	_processUser = 0;
	_processKernel = 0;

	_processor_LastKernel.QuadPart = 0;
	_processor_LastUser.QuadPart = 0;
	_processor_LastIdle.QuadPart = 0;

	_process_LastKernel.QuadPart = 0;
	_process_LastUser.QuadPart = 0;
	_process_LastTime.QuadPart = 0;

	UpdateCpuTime();
}

void CpuUsage::UpdateCpuTime()
{
	// CPU사용률 갱신
	ULARGE_INTEGER kernel;
	ULARGE_INTEGER user;
	ULARGE_INTEGER idle;

	if (GetSystemTimes((PFILETIME)&idle, (PFILETIME)&kernel, (PFILETIME)&user) == false)
		return;

	// 커널타임에는 아이들 타임 포함, 처음계산 이상함
	ULONGLONG kernelDiff = kernel.QuadPart - _processor_LastKernel.QuadPart;
	ULONGLONG userDiff = user.QuadPart - _processor_LastUser.QuadPart;
	ULONGLONG idleDiff = idle.QuadPart - _processor_LastIdle.QuadPart;

	ULONGLONG Total = kernelDiff + userDiff;

	if (Total == 0)
	{
		_processUser = 0.0f;
		_processKernel = 0.0f;
		_processTotal = 0.0f;
	}
	else
	{
		// 커널에서 아이들빼서 계산
		_processorTotal = (float)((double)(Total - idleDiff) / Total * 100.0f);
		_processorUser = (float)((double)userDiff / Total * 100.0f);
		_processorKernel = (float)((double)(kernelDiff - idleDiff) / Total * 100.0f);

		_processor_LastKernel = kernel;
		_processor_LastUser = user;
		_processor_LastIdle = idle;
	}

	// 지정된 프로세스 사용률을 갱신한다
	ULARGE_INTEGER none;
	ULARGE_INTEGER nowTime;

	GetSystemTimeAsFileTime((LPFILETIME)&nowTime);
	GetProcessTimes(_process, (LPFILETIME)&none, (LPFILETIME)&none, (LPFILETIME)&kernel, (LPFILETIME)&user);

	// 이전의 시간과 비교해서 지난시간 확인
	ULONGLONG TimeDiff = nowTime.QuadPart - _process_LastTime.QuadPart;
	userDiff = user.QuadPart - _process_LastUser.QuadPart;
	kernelDiff = kernel.QuadPart - _process_LastKernel.QuadPart;

	Total = kernelDiff + userDiff;

	_processTotal = (float)(Total / (double)_numberOfProcessors / (double)TimeDiff * 100.0f);
	_processKernel = (float)(kernelDiff / (double)_numberOfProcessors / (double)TimeDiff * 100.0f);
	_processUser = (float)(userDiff / (double)_numberOfProcessors / (double)TimeDiff * 100.0f);

	_process_LastTime = nowTime;
	_process_LastKernel = kernel;
	_process_LastUser = user;
}

