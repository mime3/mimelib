#include "pch.h"
#include "MonitorLanClient.h"

MonitorLanClient::MonitorLanClient(const char * config, TCHAR * processName, int ID) : LanClient(config)
{
	_configParser.OpenFile(config);
	_configParser.GetValue_Int("MONITOR_NO", _myID, "SERVER");
	_configParser.CloseFile();

	//SetMyID(ID);
	_masterMode = false;
	_pdh.SetProcessName(processName);
	_connected = false;

	_pdh.Add(PDH::NONPAGED_BYTES);
	_pdh.Add(PDH::AVAILABLE_MBYTES);
	_pdh.Add(PDH::PROCESS_MEMORY);
	_pdh.Add(PDH::NETWORK);

	_connectEvent[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	_connectEvent[1] = CreateWaitableTimer(NULL, FALSE, NULL);
	
	LARGE_INTEGER startWaitTime;
	startWaitTime.QuadPart = -3 * 1000 * 1000;

	SetWaitableTimer(_connectEvent[1], &startWaitTime, 1000, NULL, NULL, FALSE);
	_monitorThread = (HANDLE)_beginthreadex(NULL, 0, MonitorThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
}


MonitorLanClient::~MonitorLanClient()
{
	// apc큐
	QueueUserAPC(NotifyExit, _monitorThread, NULL);
	WaitForSingleObject(_monitorThread, INFINITE);
	CloseHandle(_connectEvent);
	CloseHandle(_monitorThread);
}

void MonitorLanClient::SetMyID(int id)
{
	_myID = id;
}

void MonitorLanClient::SetMasterMode()
{
	_masterMode = true;
}

void MonitorLanClient::MakePacket_Monitor_Login(StreamBuffer * packet, int serverNo)
{
	*packet << (WORD)en_PACKET_SS_MONITOR_LOGIN << serverNo;
}

void MonitorLanClient::MakePacket_Monitor_Data_Update(StreamBuffer * packet, BYTE dataType, int dataValue, int timeStamp)
{
	*packet << (WORD)en_PACKET_SS_MONITOR_DATA_UPDATE << dataType << dataValue << timeStamp;
}

void MonitorLanClient::NotifyExit(ULONG_PTR param)
{
	printf("monitor lan client exiting..\n");
}

unsigned int __stdcall MonitorLanClient::MonitorThreadMain(LPVOID lpParam)
{
	MonitorLanClient * _this = (MonitorLanClient *)lpParam;
	while (1)
	{
		// 종료는 APC로 확인
		DWORD ret = WaitForMultipleObjectsEx(2, _this->_connectEvent, TRUE, INFINITE, TRUE);
		if (ret == WAIT_IO_COMPLETION)
		{
			break;
		}

		time_t curTime = time(NULL);
		// pdh수집
		_this->_pdh.Update();
		// 서버로 데이터 전송
		// 가상함수를 실행시키고 상속받아서 커스텀하는게 좋음
		if (_this->_masterMode)
		{
			StreamBuffer * recvBytePacket = PacketAlloc(LanClient);
			_this->MakePacket_Monitor_Data_Update(recvBytePacket, dfMONITOR_DATA_TYPE_SERVER_NETWORK_RECV, (int)_this->_pdh.dataArray[1].value / 1024, (int)curTime);
			_this->SendPacket(_this->_serverID, recvBytePacket);
			PacketFree(recvBytePacket);

			StreamBuffer * sendBytePacket = PacketAlloc(LanClient);
			_this->MakePacket_Monitor_Data_Update(sendBytePacket, dfMONITOR_DATA_TYPE_SERVER_NETWORK_SEND, (int)_this->_pdh.dataArray[1 + _this->_pdh.NetDeviceCount].value / 1024, (int)curTime);
			_this->SendPacket(_this->_serverID, sendBytePacket);
			PacketFree(sendBytePacket);

			StreamBuffer * processorTotalPacket = PacketAlloc(LanClient);
			_this->MakePacket_Monitor_Data_Update(processorTotalPacket, dfMONITOR_DATA_TYPE_SERVER_CPU_TOTAL, (int)_this->_pdh.cpuUsage.ProcessorTotal(), (int)curTime);
			_this->SendPacket(_this->_serverID, processorTotalPacket);
			PacketFree(processorTotalPacket);

			//StreamBuffer * processorUserPacket = PacketAlloc(LanClient);
			//_this->MakePacket_Monitor_Data_Update(processorUserPacket, PROCESSOR_CPU_USER, (int)_this->_pdh.cpuUsage.ProcessorUser(), (int)curTime);
			//_this->SendPacket(_this->_serverID, processorUserPacket);
			//PacketFree(processorUserPacket);

			//StreamBuffer * processorKernelPacket = PacketAlloc(LanClient);
			//_this->MakePacket_Monitor_Data_Update(processorKernelPacket, PROCESSOR_CPU_KERNEL, (int)_this->_pdh.cpuUsage.ProcessorKernel(), (int)curTime);
			//_this->SendPacket(_this->_serverID, processorKernelPacket);
			//PacketFree(processorKernelPacket);

			StreamBuffer * nonPagePacket = PacketAlloc(LanClient);
			_this->MakePacket_Monitor_Data_Update(nonPagePacket, dfMONITOR_DATA_TYPE_SERVER_NONPAGED_MEMORY, (int)(_this->_pdh.dataValue[0].value / 1024 / 1024), (int)curTime);
			_this->SendPacket(_this->_serverID, nonPagePacket);
			PacketFree(nonPagePacket);

			StreamBuffer * memPacket = PacketAlloc(LanClient);
			_this->MakePacket_Monitor_Data_Update(memPacket, dfMONITOR_DATA_TYPE_SERVER_AVAILABLE_MEMORY, (int)(_this->_pdh.dataValue[1].value), (int)curTime);
			_this->SendPacket(_this->_serverID, memPacket);
			PacketFree(memPacket);
		}

		_this->OnUpdate();
	}
	return 0;
}

void MonitorLanClient::OnClientJoin(INT64 ClientID, Session * session)
{
}

void MonitorLanClient::OnClientLeave(INT64 ClientID)
{
	ResetEvent(_connectEvent[0]);
	_connected = false;
	Reconnect();
}

bool MonitorLanClient::OnConnectionRequest(char * ClientIP, int Port)
{
	return true;
}

void MonitorLanClient::OnRecv(INT64 clientID, StreamBuffer * packet)
{
}

void MonitorLanClient::OnSend(INT64 clientID, int sendSize)
{
}

void MonitorLanClient::OnError(int errorCode, const WCHAR * errorMessage)
{
}

void MonitorLanClient::OnConnect(INT64 ClientID)
{
	_serverID = ClientID;
	_connected = true;

	//login
	StreamBuffer * loginPacket = PacketAlloc(LanClient);
	MakePacket_Monitor_Login(loginPacket, _myID);
	SendPacket(ClientID, loginPacket);
	PacketFree(loginPacket);

	SetEvent(_connectEvent[0]);
}
