#include "pch.h"
#include "PDHClass.h"


PDH::PDH()
{
	CounterIndex = 0;
	Status = PdhOpenQuery(NULL, NULL, &Query);
	if (Status != ERROR_SUCCESS)
	{
		wprintf(L"\nPdhOpenQuery failed with status 0x%x.", Status);
	}
	ZeroMemory(&CounterPathBuffer, sizeof(CounterPathBuffer));

}

PDH::~PDH()
{
	if (Query)
	{
		PdhCloseQuery(Query);
	}
}

void PDH::SetProcessName(WCHAR * processName)
{
	WCHAR * search = processName;
	while (1)
	{
		WCHAR * str = wcsstr(search, L"\\");
		if (str == NULL)
		{
			WCHAR * temp;
			search = wcstok_s(search, L".", &temp);
			break;
		}
		else
			search = str + 1;
	}
	wcscpy_s(ProcessName, search);
}

bool PDH::Add(int counter)
{
	switch (counter)
	{
	case NONPAGED_BYTES:
		swprintf_s(CounterPathBuffer, PDH_MAX_COUNTER_PATH, L"\\Memory\\Pool Nonpaged Bytes");
		break;
	case AVAILABLE_MBYTES:
		swprintf_s(CounterPathBuffer, PDH_MAX_COUNTER_PATH, L"\\Memory\\Available MBytes");	
		break;
	case NETWORK:
		swprintf_s(CounterPathBuffer, PDH_MAX_COUNTER_PATH, L"\\Network Interface(*)\\Bytes Received/sec");
		PdhAddCounter(Query, CounterPathBuffer, 0, &NetCounter[0]);
		swprintf_s(CounterPathBuffer, PDH_MAX_COUNTER_PATH, L"\\Network Interface(*)\\Bytes Sent/sec");
		PdhAddCounter(Query, CounterPathBuffer, 0, &NetCounter[1]);
		return true;
	case PROCESS_MEMORY:
		swprintf_s(CounterPathBuffer, PDH_MAX_COUNTER_PATH, L"\\Process(%s)\\Private Bytes", ProcessName);
		break;
	}

	Status = PdhAddCounter(Query, CounterPathBuffer, 0, &Counter[CounterIndex]);
	
	if (Status != ERROR_SUCCESS)
	{
		wprintf(L"\nPdhAddCounter failed with status 0x%x.", Status);
		return false;
	}

	Status = PdhCollectQueryData(Query);
	if (Status != ERROR_SUCCESS)
	{
		wprintf(L"\nPdhCollectQueryData failed with 0x%x.\n", Status);	
		return false;
	}

	CounterIndex++;
	return true;
}


void PDH::Update()
{
	ZeroMemory(dataArray, sizeof(dataArray));
	ZeroMemory(dataValue, sizeof(dataValue));

	Status = PdhCollectQueryData(Query);
	if (Status != ERROR_SUCCESS)
	{
		wprintf(L"\nPdhCollectQueryData failed with status 0x%x.", Status);
		return;
	}

	for (DWORD i = 0; i < CounterIndex; i++)
	{
		Status = PdhGetFormattedCounterValue(Counter[i], PDH_FMT_LONG, &CountType, &DisplayValue);
		if (Status != ERROR_SUCCESS)
		{
			wprintf(L"PdhGetFormattedCounterValue failed with 0x%x.\n", Status);
			return;
		}
		dataValue[i].value = DisplayValue.longValue;
	}

	Status = PdhGetFormattedCounterArray(NetCounter[0], PDH_FMT_LONG, &BufferSize, &ItemCount, item);
	if (Status == PDH_MORE_DATA)
	{
		item = (PDH_FMT_COUNTERVALUE_ITEM *)malloc(BufferSize);
		Status = PdhGetFormattedCounterArray(NetCounter[0], PDH_FMT_LONG, &BufferSize, &ItemCount, item);
		for (DWORD i = 0; i < ItemCount; i++)
		{
			wcscpy_s(dataArray[i].name, 62, item[i].szName);
			dataArray[i].value = item[i].FmtValue.longValue;
		}
		free(item);
		item = nullptr;
		NetDeviceCount = ItemCount;
		BufferSize = ItemCount = 0;
	}

	Status = PdhGetFormattedCounterArray(NetCounter[1], PDH_FMT_LONG, &BufferSize, &ItemCount, item);
	if (Status == PDH_MORE_DATA)
	{
		item = (PDH_FMT_COUNTERVALUE_ITEM *)malloc(BufferSize);
		Status = PdhGetFormattedCounterArray(NetCounter[1], PDH_FMT_LONG, &BufferSize, &ItemCount, item);
		for (DWORD i = 0; i < ItemCount; i++)
		{
			wcscpy_s(dataArray[i + ItemCount].name, 62, item[i].szName);
			dataArray[i + ItemCount].value = item[i].FmtValue.longValue;
		}
		free(item);
		item = nullptr;
		BufferSize = ItemCount = 0;
	}

	cpuUsage.UpdateCpuTime();
}
