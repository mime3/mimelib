#ifndef __MINLIB_PDH_CLASS__
#define __MINLIB_PDH_CLASS__
#pragma once
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <Pdh.h>
#include <PdhMsg.h>
#include <Psapi.h>
#include <stdlib.h>
#include "CpuUsage.h"

#pragma comment(lib, "pdh.lib")
namespace MinLib
{
	struct PDHDATA
	{
		ULONG value;
		WCHAR name[62];
	};

	struct PDHDATA_VALUE
	{
		ULONG value;
	};

	class PDH
	{
		PDH_STATUS				Status;
		HQUERY					Query = NULL;
		HCOUNTER				NetCounter[2];
		HCOUNTER				Counter[10];
		int						CounterIndex;
		int						NetDeviceCount;
		PDH_FMT_COUNTERVALUE	DisplayValue;
		DWORD					CountType;
		DWORD					BufferSize;
		DWORD					ItemCount;
		PDH_FMT_COUNTERVALUE_ITEM* item;
		WCHAR					CounterPathBuffer[PDH_MAX_COUNTER_PATH];

	public:
		PDHDATA					dataArray[8];
		PDHDATA_VALUE			dataValue[8];
		CpuUsage				cpuUsage;
		WCHAR					ProcessName[64];

	public:
		PDH();
		~PDH();

		void SetProcessName(WCHAR* processName);

		bool Add(int counter);
		void Update();

		enum COUNTER
		{
			NONPAGED_BYTES = 1,
			AVAILABLE_MBYTES = 2,
			NETWORK = 3,
			PROCESS_MEMORY = 4
		};

		friend class MonitorLanClient;
	};
}
#endif // !__MINLIB_PDH_CLASS__