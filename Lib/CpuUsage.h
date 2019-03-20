#pragma once
#include <Windows.h>
class CpuUsage
{
	HANDLE _process;
	int _numberOfProcessors;

	float _processorTotal;
	float _processorUser;
	float _processorKernel;

	float _processTotal;
	float _processUser;
	float _processKernel;

	ULARGE_INTEGER _processor_LastKernel;
	ULARGE_INTEGER _processor_LastUser;
	ULARGE_INTEGER _processor_LastIdle;

	ULARGE_INTEGER _process_LastKernel;
	ULARGE_INTEGER _process_LastUser;
	ULARGE_INTEGER _process_LastTime;

public:
	CpuUsage(HANDLE process = INVALID_HANDLE_VALUE);

	void UpdateCpuTime();

	float ProcessorTotal() { return _processorTotal; }
	float ProcessorUser() { return _processorUser; }
	float ProcessorKernel() { return _processorKernel; }

	float ProcessTotal() { return _processTotal; }
	float ProcessUser() { return _processUser; }
	float ProcessKernel() { return _processKernel; }

};

