#pragma once

struct PacketHeader
{
	BYTE code;	// 패킷코드 0x89 고정.
	BYTE size;	// 패킷 사이즈.
	BYTE type;	// 패킷타입.
	BYTE temp;	// 사용안함.
};

struct Session
{
	Session();

	SOCKET sock;
	SOCKADDR_IN addr;
	MinLib::StreamQueue * resvStream;
	MinLib::StreamQueue * sendStream;
	bool sendFlag;
	bool connected;
	bool headerSaveFlag;
	PacketHeader saveHeader;
	u_int totalRecv;
	u_int totalSend;
};
