#pragma once
#include <Windows.h>
#include <queue>

#include "StreamBuffer.h"
#include "StreamQueue.h"
#include "LockFreeQueue.h"

constexpr int mmoSendArraySize = 200;
enum MODE
{
	NONE,
	AUTH,
	AUTH_TO_CONTENTS,
	CONTENTS,
	LOGOUT_IN_AUTH,
	LOGOUT_IN_CONTENTS,
	WAIT_LOGOUT
};

struct ACCEPT_INFO
{
	SOCKET socket;
	WCHAR IP[16];
	int port;
};

class MMOSession
{
public:
#pragma pack(push, 1)   
	struct PACKET_HEADER
	{
		BYTE code;
		WORD len;
		BYTE randXORKey;
		BYTE checkSum;
	};
#pragma pack(pop)
	static BYTE	_packetCode;
	static BYTE	_XORKey1;
	static BYTE	_XORKey2;

private:
	SOCKET _socket;
	WCHAR _IP[16];
	int _port;
	INT64 _sessionID;
	int _ioCount;
	MODE _mode;

	OVERLAPPED _sendOverLapped;					// sendoverlapped
	OVERLAPPED _recvOverLapped;					// recvoverlapped
	BOOL _sendFlag;								// 전송중 여부 플래그
	int _sendCount;								// 전송한 패킷 수
	LF_Queue<StreamBuffer *> _sendQueue;
	StreamQueue	_recvQueue;
	StreamBuffer * _sendArray[mmoSendArraySize];	// 전송한 패킷 저장배열
	//std::queue<StreamBuffer *> _completeRecvQueue;
	LF_Queue<StreamBuffer *> _completeRecvQueue;

	virtual void OnAuth_Packet(StreamBuffer * packet) = 0;
	virtual void OnAuth_ClientLeave(bool isExit) = 0;
	virtual void OnContents_Packet(StreamBuffer * packet) = 0;
	virtual void OnContents_ClientJoin() = 0;
	virtual void OnContents_ClientLeave() = 0;
	virtual void OnContents_ClientRelease() = 0;

protected:
	bool _logOutFlag;
	bool _authToContentsFlag;
	
	inline BYTE GetCheckSum(char * buffer, int size);
	inline void XOR(char * buffer, int size, char key);
	inline void Encode(StreamBuffer * packet);
	inline void PutHeader(StreamBuffer * packet);
	
public:
	MMOSession();
	~MMOSession();
	void Init(ACCEPT_INFO * acceptInfo, INT64 sessionID);
	void Clean();
	bool SendPacket(StreamBuffer * packet);
	friend class MMOServer;
};

