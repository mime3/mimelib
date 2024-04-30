#pragma once
#include "stdafx.h"

extern multimap<TAG_ACTION, SECTOR_POS> g_moveInsertTable;
extern multimap<TAG_ACTION, SECTOR_POS> g_moveDeleteTable;
extern map<SOCKET, Session *> g_user_socket;
extern map<SOCKET, Session *> g_deleteUser_socket;
extern list<Player *> g_playerList;
extern int g_ID;

/*----------------------------------------------------------*/
// ::Init
// 설명 : 네트워크 세팅함수
//  ㄴ  : 리슨소켓을 세팅한다.
// 인자 : (SOCKET *) 리슨소켓 포인터
// 리턴 : (bool) 성공여부
/*----------------*////////////////////////*----------------*/
bool Init(SOCKET * listenSocket)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	int retval;
	SOCKADDR_IN addr;
	*listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	char reuseaddr = 1;
	setsockopt(*listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, (PCSTR)INADDR_ANY, &addr.sin_addr.s_addr);
	addr.sin_port = htons(NETWORK_PORT);
#ifdef TCPNODELAY
	char tcpNoDelay = 1;
	setsockopt(*listenSocket, IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(tcpNoDelay));
#endif // TCPNODELAY
	retval = bind(*listenSocket, (SOCKADDR*)&addr, sizeof(addr));
	if (retval == SOCKET_ERROR)
	{
		LOG(Logger::LOG_LEVEL_ERROR, L"bind Error\n");
		system("pause");
		return false;
	}
	listen(*listenSocket, SOMAXCONN);
	TableInit();
	LOG(Logger::LOG_LEVEL_ERROR, L"Server OPEN");
	Logger::GetInstance().SetLogLevel(Logger::LOG_LEVEL_WARNING);
	return true;
}

/*----------------------------------------------------------*/
// ::TableInit
// 설명 : 전역 이동테이블 멀티맵 세팅함수
//  ㄴ  : 플레이어의 이동시 신규섹터, 삭제섹터의 정보를 담는다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void TableInit()
{
	// 추가로 정보가 필요한 섹터의 상대위치, 기준은 새 섹터
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{-1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{-1, 0}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{-1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{-1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{-1, 0}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{-1, +1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{0, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{-1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{0, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{-1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{0, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{1, 0}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{1, 0}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{-1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{0, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{1, 0}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{-1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{0, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{-1, -1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{-1, 0}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{-1, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{0, 1}));
	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{1, 1}));
	// 삭제되야할 정보의 상대섹터위치, 기준은 원래섹터
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{ 1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{ 1, 0 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{ 1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ -1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 0, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 1, 0 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{ -1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{ 0, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{ 1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ -1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ -1, 0 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ -1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ 0, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ 1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{ -1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{ -1, 0 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{ -1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ -1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ -1, 0 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ -1, 1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ 0, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ 1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{ -1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{ 0, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{ 1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ -1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 0, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 1, -1 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 1, 0 }));
	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 1, 1 }));
}

/*----------------------------------------------------------*/
// ::NetworkingEx
// 설명 : 네트워크 Select함수부분
// 인자 : (SOCKET) 리슨소켓
// 리턴 :
/*----------------*////////////////////////*----------------*/
void NetworkingEx(SOCKET listenSocket)
{
	FD_SET readSet;
	FD_SET writeSet;
	map<SOCKET, Session *>::iterator iter = g_user_socket.begin();
	while (true)
	{
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		FD_SET(listenSocket, &readSet);
		for (int j = 0; j < FD_SETSIZE - 1; ++j)
		{
			if (iter == g_user_socket.end())
				break;
			FD_SET(iter->second->socket, &readSet);
			if (0 < iter->second->sendStream.GetUseSize())
				FD_SET(iter->second->socket, &writeSet);
			++iter;
		}

		timeval waitTime;
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 0;
		int retval = select(0, &readSet, &writeSet, NULL, &waitTime);

		if (retval == 0 || retval == SOCKET_ERROR)
		{
			if (iter == g_user_socket.end())
				break;
			continue;
		}

		for (UINT i = 0; i < writeSet.fd_count; ++i)
		{
			SendProc(writeSet.fd_array[i]);
		}

		for (UINT i = 0; i < readSet.fd_count; ++i)
		{
			if (listenSocket == readSet.fd_array[i])
			{
				SOCKET client;
				SOCKADDR_IN addr;
				int addrSize = sizeof(addr);
				client = accept(listenSocket, (SOCKADDR*)&addr, &addrSize);
				LoginProcess(client, addr);
			}
			else
				RecvProc(readSet.fd_array[i]);
		}

		// 루프 종료
		if (iter == g_user_socket.end())
			break;
	}
}

/*----------------------------------------------------------*/
// ::RecvProc
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
bool RecvProc(SOCKET recvSocket)
{
	Session * sendSession = g_user_socket[recvSocket];

	// 한번에 저장가능한 양이나 최대 2000바이트
	int size = min(sendSession->recvStream.GetNotBrokenPutSize(), 2000);
	int received = recv(sendSession->socket, sendSession->recvStream.GetWriteBufferPtr(), size, 0);
	// 종료 메시지
	if (received == 0 || received == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (received == SOCKET_ERROR)
		{
			LOG(Logger::LOG_LEVEL_DEBUG, L"Socket Error Code :%d", error);
			TCHAR * message = nullptr;
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				nullptr,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(TCHAR *)&message,
				0,
				nullptr);
			LOG(Logger::LOG_LEVEL_DEBUG, L"%ws", message);
			LocalFree(message);
		}
		LogoutProcess(recvSocket);
		return false;
	}
	sendSession->recvStream.MoveWritePos(received);
	return true;
}

/*----------------------------------------------------------*/
// ::SendProc
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void SendProc(SOCKET sendSocket)
{
	Session * sendSession = g_user_socket[sendSocket];
	while (1)
	{
		int sendSize = sendSession->sendStream.GetNotBrokenGetSize();
		// send 링버퍼가 비워짐
		if (sendSize == 0)
			break;
		// blockSocket이기 때문에 sendSize == retval이 아니면 블럭상태가되며 무조건 같을때만 리턴
		int retval = send(sendSession->socket, sendSession->sendStream.GetReadBufferPtr(), sendSize, 0);
		if (retval == SOCKET_ERROR)
			break;
		else
			sendSession->sendStream.RemoveData(retval);
	}
}

/*----------------------------------------------------------*/
// ::PacketRecv
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void PacketRecv()
{
	map<SOCKET, Session *>::iterator iter = g_user_socket.begin();
	map<SOCKET, Session *>::iterator iterEnd = g_user_socket.end();
	for (; iter != iterEnd;)
	{
		while (true)
		{
			st_PACKET_HEADER packetHeader;
			int received;
			if (iter->second->saveHeaderFlag == true)
			{
				packetHeader = iter->second->saveHeader;
				iter->second->saveHeaderFlag = false;
			}
			else
			{
				received = iter->second->recvStream.Peek((char *)&packetHeader, sizeof(packetHeader));
				if (received < sizeof(packetHeader))
					break;
				iter->second->recvStream.RemoveData(received);
				// 패킷코드 검증
				if (packetHeader.byCode != dfPACKET_CODE)
				{
					CloseSession(iter->second);
					delete iter->second->character;
					delete iter->second;
					iter = g_user_socket.erase(iter);
					if (iter == g_user_socket.end())
						return;
					--iter;
					break;
				}
			}

			// 헤더는 받았지만 payload가 모두 도착하지 않았다.
			BYTE endCode;
			if (iter->second->recvStream.GetUseSize() < packetHeader.bySize + sizeof(endCode))
			{
				iter->second->saveHeaderFlag = true;
				iter->second->saveHeader = packetHeader;
				break;
			}

			MinLib::StreamBuffer payLoad(packetHeader.bySize);
			received = iter->second->recvStream.Dequeue(payLoad.GetBuffer(), packetHeader.bySize);
			payLoad.MoveEndIndex();

			int retval = iter->second->recvStream.Dequeue((char *)&endCode, sizeof(endCode));
			if (retval != sizeof(endCode))
			{
				LogoutProcess(iter->second->socket);
				LOG(Logger::LOG_LEVEL_ERROR, L"EndCode Error Socket %d\n", iter->second->socket);
			}
			else
			{
				if (endCode == dfNETWORK_PACKET_END)
					Parsing(iter->second, packetHeader.byType, &payLoad);
				else
				{
					LogoutProcess(iter->second->socket);
					LOG(Logger::LOG_LEVEL_ERROR, L"EndCode Error Socket %d\n", iter->second->socket);
				}
			}
		}
		++iter;
	}
}

/*----------------------------------------------------------*/
// ::LoginProcess
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
bool LoginProcess(SOCKET socket, SOCKADDR_IN addr)
{
	Session * newSession = new Session(socket, addr);
	Player * newPlayer = new Player(newSession, g_ID);
	if (newSession == nullptr || newPlayer == nullptr)
	{
		LOG(Logger::LOG_LEVEL_ERROR, L"new operator error");
		return false;
	}
	MinLib::StreamBuffer * packet;
	MakePacket_Create_My_Character(&packet, g_ID, ACTION_MOVE_LL, newPlayer->_pos, newPlayer->_healthPoint);
	UniCast(packet, newSession);
	// 새 유저에 기존유저 목록을, 기존유저에게 새 유저를
	SECTOR_MANAGER.NewPlayer(newPlayer);
	g_ID++;
	return true;
}

/*----------------------------------------------------------*/
// ::LogoutProcess
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void LogoutProcess(SOCKET socket)
{
	// 섹터에서 삭제
	Session * logout = g_user_socket[socket];
	SECTOR_MANAGER.DeletePlayer(logout->character);
	// 세션리스트에서 삭제
	g_user_socket.erase(socket);
	// 삭제예정 세션리스트에 삽입
	g_deleteUser_socket.insert(pair<SOCKET, Session *>(socket, logout));

	//// 소켓닫기
	//closesocket(socket);
	//// 해당 유저 나감을 섹터의 유저에게 알림
	//Session * logout = g_user_socket[socket];
	//MinLib::StreamBuffer * packet;
	//MakePacket_Delete_Character(&packet, logout->character->GetID());
	//BroadCast_Sector(packet, logout);

	//LOG(Logger::LOG_LEVEL_DEBUG, L"DisConnect %Id\n", socket);
	//delete logout->character;
	//delete logout;
}

/*----------------------------------------------------------*/
// ::Parsing
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Parsing(Session * session, WORD message, MinLib::StreamBuffer * payload)
{
	switch (message)
	{
	case dfPACKET_CS_MOVE_START:
		RecvPacket_CS_Move_Start(session, payload);
		break;
	case dfPACKET_CS_MOVE_STOP:
		RecvPacket_CS_Move_Stop(session, payload);
		break;
	case dfPACKET_CS_ATTACK1:
		RecvPacket_CS_Attack1(session, payload);
		break;
	case dfPACKET_CS_ATTACK2:
		RecvPacket_CS_Attack2(session, payload);
		break;
	case dfPACKET_CS_ATTACK3:
		RecvPacket_CS_Attack3(session, payload);
		break;
	case dfPACKET_CS_ECHO:
		RecvPacket_CS_Echo(session, payload);
		break;
	default:
		LOG(Logger::LOG_LEVEL_WARNING, L"wrong packet");
		break;
	}
	session->lastRecvTime = GetTickCount64();
}

/*----------------------------------------------------------*/
// ::CloseSession
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void CloseSession(Session * session)
{
	closesocket(session->socket);
}

/*----------------------------------------------------------*/
// ::CheckDeadReckoning
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
bool CheckDeadReckoning(Player * player, Pos pos)
{
	if (DEADRECKONING_RANGE < abs(player->_pos.x - pos.x))
		return true;
	else if (DEADRECKONING_RANGE < abs(player->_pos.y - pos.y))
		return true;
	return false;
}

/*----------------------------------------------------------*/
// ::DeleteSession
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void DeleteSession()
{
	map<SOCKET, Session *>::iterator iter = g_deleteUser_socket.begin();
	map<SOCKET, Session *>::iterator iterEnd = g_deleteUser_socket.end();
	for (; iter != iterEnd;)
	{
		MinLib::StreamBuffer * deleteUser;
		MakePacket_Delete_Character(&deleteUser, iter->second->character->_ID);
		BroadCast_Sector(deleteUser, iter->second);
		LOG(Logger::LOG_LEVEL_DEBUG, L"DisConnect %Id\n", iter->second->character->_ID);
		delete iter->second->character;
		delete iter->second;
		iter = g_deleteUser_socket.erase(iter);
	}
}

Pos DeadReckoning(Session * session)
{
	ULONGLONG time = GetTickCount64() - session->character->_lastActionTime;
	time /= 20;
	short x = session->character->_moveStartPos.x;
	short y = session->character->_moveStartPos.y;
	Pos oldPos = { x, y };
	for (int i = 0; i < time; ++i)
	{
		switch (session->character->_action)
		{
		case ACTION_MOVE_LL:
			x -= MOVE_TICK_X;
			break;
		case ACTION_MOVE_LU:
			x -= MOVE_TICK_X;
			y -= MOVE_TICK_Y;
			break;
		case ACTION_MOVE_UU:
			y -= MOVE_TICK_Y;
			break;
		case ACTION_MOVE_RU:
			x += MOVE_TICK_X;
			y -= MOVE_TICK_Y;
			break;
		case ACTION_MOVE_RR:
			x += MOVE_TICK_X;
			break;
		case ACTION_MOVE_RD:
			x += MOVE_TICK_X;
			y += MOVE_TICK_Y;
			break;
		case ACTION_MOVE_DD:
			y += MOVE_TICK_Y;
			break;
		case ACTION_MOVE_LD:
			x -= MOVE_TICK_X;
			y += MOVE_TICK_Y;
			break;
		default:
			return Pos{ x, y };
		}
		if (x < 0 || MAP_X_SIZE < x)
			return Pos{ oldPos.x, oldPos.y };
		if (y < 0 || MAP_Y_SIZE < y)
			return Pos{ oldPos.x, oldPos.y };

		oldPos.x = x;
		oldPos.y = y;
	}
	return Pos{ x, y };
}