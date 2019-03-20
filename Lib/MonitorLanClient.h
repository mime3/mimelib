#pragma once
#include "LanClient.h"
#include "PDHClass.h"
//#include "CommonProtocol.h"

enum DATA_TYPE_NUM
{
	PROCESSOR_CPU_TOTAL,
	PROCESSOR_CPU_KERNEL,
	PROCESSOR_CPU_USER,
	PROCESS_CPU_TOTAL,
	PROCESS_CPU_KERNEL,
	PROCESS_CPU_USER,
	RECVBYTE,
	SENDBYTE,
	NONPAGE,
	MEMORY,
	PROCESS_MEMORY
};

class MonitorLanClient : public LanClient
{
	Parser _configParser;
	bool _masterMode;
	HANDLE _connectEvent[2];
	HANDLE _monitorThread;

protected:
	bool _connected;
	int _myID;
	INT64 _serverID;
	PDH	_pdh;

public:
	MonitorLanClient(const char * config, TCHAR * processName, int ID = 0);
	//종료는 APC큐로 종료
	~MonitorLanClient();

	void SetMyID(int id);
	void SetMasterMode();

	void MakePacket_Monitor_Login(StreamBuffer * packet, int serverNo);
	void MakePacket_Monitor_Data_Update(StreamBuffer * packet, BYTE dataType, int dataValue, int timeStamp);

	static void CALLBACK NotifyExit(ULONG_PTR param);
	// 모니터 스레드로 매 서버에 업데이트 송신
	static unsigned int WINAPI MonitorThreadMain(LPVOID lpParam);

	// LanClient을(를) 통해 상속됨
	virtual void OnClientJoin(INT64 ClientID, Session * session) override;
	virtual void OnClientLeave(INT64 ClientID) override;
	virtual bool OnConnectionRequest(char * ClientIP, int Port) override;
	virtual void OnRecv(INT64 clientID, StreamBuffer * packet) override;
	virtual void OnSend(INT64 clientID, int sendSize) override;
	virtual void OnError(int errorCode, const WCHAR * errorMessage) override;
	virtual void OnConnect(INT64 ClientID) override;
	
	virtual void OnUpdate() = 0;
};

