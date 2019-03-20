#include "pch.h"
#include "LoginMonitorClient.h"

void LoginMonitorClient::OnUpdate()
{
	time_t curTime = time(NULL);

	StreamBuffer * sessionTotal = PacketAlloc(LanServer);
	MakePacket_Monitor_Data_Update(sessionTotal,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_LOGIN_SESSION,
		_netPointer->GetClientCount(), (int)curTime);
	SendPacket(_serverID, sessionTotal);
	PacketFree(sessionTotal);

	StreamBuffer * authTPS = PacketAlloc(LanServer);
	MakePacket_Monitor_Data_Update(authTPS,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_LOGIN_AUTH_TPS,
		_netPointer->_acceptTPS, (int)curTime);
	SendPacket(_serverID, authTPS);
	PacketFree(authTPS);

	StreamBuffer * packetPool = PacketAlloc(LanServer);
	MakePacket_Monitor_Data_Update(packetPool,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_LOGIN_PACKET_POOL,
		StreamBuffer::_allocCount, (int)curTime);
	SendPacket(_serverID, packetPool);
	PacketFree(packetPool);

	StreamBuffer * serverOn = PacketAlloc(LanServer);
	MakePacket_Monitor_Data_Update(serverOn,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_LOGIN_SERVER_ON,
		_lanPointer->GetClientCount(), (int)curTime);
	SendPacket(_serverID, serverOn);
	PacketFree(serverOn);

	StreamBuffer * liveServer = PacketAlloc(LanServer);
	MakePacket_Monitor_Data_Update(liveServer,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_LOGIN_LIVE_SERVER,
		0, (int)curTime);
	//SendPacket(_serverID, liveServer);
	PacketFree(liveServer);


}

LoginMonitorClient::LoginMonitorClient(const char * configFile, WCHAR * processName, int ID) : MonitorLanClient(configFile, processName, ID)
{
}
