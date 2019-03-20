#include "pch.h"
#include "ChatMonitorClient.h"

ChatMonitorClient::ChatMonitorClient(const char * configFile, WCHAR * processName, int ID) : MonitorLanClient(configFile, processName, ID)
{
}

void ChatMonitorClient::OnUpdate()
{
	time_t curTime = time(NULL);
	/*
dfMONITOR_DATA_TYPE_CHAT_SERVER_ON,                         // ä�ü��� ON
	dfMONITOR_DATA_TYPE_CHAT_CPU,                           // ä�ü��� CPU ���� (Ŀ�� + ����)
	dfMONITOR_DATA_TYPE_CHAT_MEMORY_COMMIT,                 // ä�ü��� �޸� ���� Ŀ�� ��뷮 (Private) MByte
	dfMONITOR_DATA_TYPE_CHAT_PACKET_POOL,                   // ä�ü��� ��ŶǮ ��뷮
	dfMONITOR_DATA_TYPE_CHAT_SESSION,                       // ä�ü��� ���� ������ü
	dfMONITOR_DATA_TYPE_CHAT_PLAYER,                        // ä�ü��� �α����� ������ ��ü �ο�
	*/

	// ���� ��
	StreamBuffer * serverOn = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(serverOn,
		(BYTE)dfMONITOR_DATA_TYPE_CHAT_SERVER_ON,
		1,
		(int)curTime);
	SendPacket(_serverID, serverOn);
	PacketFree(serverOn);

	// ���μ��� CPU
	StreamBuffer * processCPU = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(processCPU,
		(BYTE)dfMONITOR_DATA_TYPE_CHAT_CPU,
		(int)_pdh.cpuUsage.ProcessTotal(),
		(int)curTime);
	SendPacket(_serverID, processCPU);
	PacketFree(processCPU);

	// ���μ��� Ŀ�� �޸�
	StreamBuffer * processMemory = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(processMemory,
		(BYTE)dfMONITOR_DATA_TYPE_CHAT_MEMORY_COMMIT,
		((UINT)_pdh.dataValue[2].value) / (1024 * 1024),
		(int)curTime);
	SendPacket(_serverID, processMemory);
	PacketFree(processMemory);

	// ��ŶǮ ��뷮
	StreamBuffer * packetPool = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(packetPool,
		(BYTE)dfMONITOR_DATA_TYPE_CHAT_PACKET_POOL,
		StreamBuffer::_allocCount,
		(int)curTime);
	SendPacket(_serverID, packetPool);
	PacketFree(packetPool);

	// ä�ü��� ���Ǽ�
	StreamBuffer * sessionTotal = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(sessionTotal,
		(BYTE)dfMONITOR_DATA_TYPE_CHAT_SESSION,
		(int)_netPointer->_playerMap.size(),
		(int)curTime);
	SendPacket(_serverID, sessionTotal);
	PacketFree(sessionTotal);

	// ä�ü��� �α��� ���� ����
	StreamBuffer * loginUser = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(loginUser,
		(BYTE)dfMONITOR_DATA_TYPE_CHAT_PLAYER,
		_netPointer->_LOGIN_PLAYER,
		(int)curTime);
	SendPacket(_serverID, loginUser);
	PacketFree(loginUser);

	/*
	StreamBuffer * sessionTotal = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(sessionTotal,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_CHAT_SESSION,
		_netPointer->GetClientCount(),
		(int)curTime);
	SendPacket(_serverID, sessionTotal);
	PacketFree(sessionTotal);

	StreamBuffer * playerTotal = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(playerTotal,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_CHAT_PLAYER,
		_netPointer->_playerMap.size(),
		(int)curTime);
	SendPacket(_serverID, playerTotal);
	PacketFree(playerTotal);

	StreamBuffer * updateTPS = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(updateTPS,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_CHAT_UPDATE_TPS,
		_netPointer->_UPDATE_TPS,
		(int)curTime);
	SendPacket(_serverID, updateTPS);
	PacketFree(updateTPS);

	StreamBuffer * packetPool = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(packetPool,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_CHAT_PACKET_POOL,
		StreamBuffer::_allocCount,
		(int)curTime);
	SendPacket(_serverID, packetPool);
	PacketFree(packetPool);

	StreamBuffer * msgPool = PacketAlloc(LanClient);
	MakePacket_Monitor_Data_Update(msgPool,
		(BYTE)en_PACKET_SS_MONITOR_DATA_UPDATE2::dfMONITOR_DATA_TYPE_CHAT_UPDATEMSG_POOL,
		_netPointer->_memoryPoolMessage.GetUseBlockCount(),
		(int)curTime);
	SendPacket(_serverID, msgPool);
	PacketFree(msgPool);
	*/
}
