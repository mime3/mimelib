#pragma once
#include "stdafx.h"

//extern multimap<TAG_ACTION, SECTOR_POS> g_moveInsertTable;
//extern multimap<TAG_ACTION, SECTOR_POS> g_moveDeleteTable;
//extern map<SOCKET, Session *> g_user_socket;
//extern map<SOCKET, Session *> g_deleteUser_socket;
//extern list<Player *> g_playerList;
extern Moniter moniter;
extern int g_ID;

/*----------------------------------------------------------*/
// ::Init
// ���� : ��Ʈ��ũ �����Լ�
//  ��  : ���������� �����Ѵ�.
// ���� : (SOCKET *) �������� ������
// ���� : (bool) ��������
/*----------------*////////////////////////*----------------*/
//bool Init(SOCKET * listenSocket)
//{
//	WSADATA wsaData;
//	WSAStartup(MAKEWORD(2, 2), &wsaData);
//	int retval;
//	SOCKADDR_IN addr;
//	*listenSocket = socket(AF_INET, SOCK_STREAM, 0);
//	char reuseaddr = 1;
//	setsockopt(*listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
//	ZeroMemory(&addr, sizeof(addr));
//	addr.sin_family = AF_INET;
//	inet_pton(AF_INET, INADDR_ANY, &addr.sin_addr.s_addr);
//	addr.sin_port = htons(NETWORK_PORT);
//#ifdef TCPNODELAY
//	char tcpNoDelay = 1;
//	setsockopt(*listenSocket, IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(tcpNoDelay));
//#endif // TCPNODELAY
//	retval = bind(*listenSocket, (SOCKADDR*)&addr, sizeof(addr));
//	if (retval == SOCKET_ERROR)
//	{
//		LOG(Logger::LOG_LEVEL_ERROR, L"bind Error\n");
//		system("pause");
//		return false;
//	}
//	listen(*listenSocket, SOMAXCONN);
//	TableInit();
//	LOG(Logger::LOG_LEVEL_ERROR, L"Server OPEN");
//	Logger::GetInstance().SetLogLevel(Logger::LOG_LEVEL_WARNING);
//	return true;
//}

/*----------------------------------------------------------*/
// ::TableInit
// ���� : ���� �̵����̺� ��Ƽ�� �����Լ�
//  ��  : �÷��̾��� �̵��� �űԼ���, ���������� ������ ��´�.
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
//void TableInit()
//{
//	// �߰��� ������ �ʿ��� ������ �����ġ, ������ �� ����
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{-1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{-1, 0}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{-1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{-1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{-1, 0}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{-1, +1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{0, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{-1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{0, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{-1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{0, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{1, 0}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{1, 0}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{-1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{0, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{1, 0}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{-1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{0, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{-1, -1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{-1, 0}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{-1, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{0, 1}));
//	g_moveInsertTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{1, 1}));
//	// �����Ǿ��� ������ ��뼽����ġ, ������ ��������
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{ 1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{ 1, 0 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LL, SECTOR_POS{ 1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ -1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 0, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 1, 0 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LU, SECTOR_POS{ 1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{ -1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{ 0, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_UU, SECTOR_POS{ 1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ -1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ -1, 0 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ -1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ 0, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RU, SECTOR_POS{ 1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{ -1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{ -1, 0 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RR, SECTOR_POS{ -1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ -1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ -1, 0 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ -1, 1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ 0, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_RD, SECTOR_POS{ 1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{ -1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{ 0, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_DD, SECTOR_POS{ 1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ -1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 0, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 1, -1 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 1, 0 }));
//	g_moveDeleteTable.insert(pair<TAG_ACTION, SECTOR_POS>(ACTION_MOVE_LD, SECTOR_POS{ 1, 1 }));
//}

/*----------------------------------------------------------*/
// ::NetworkingEx
// ���� : ��Ʈ��ũ Select�Լ��κ�
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void NetworkingEx()
{
	FD_SET readSet;
	FD_SET writeSet;
	map<SOCKET, Session *>::iterator iter = moniter.user_socket.begin();
	while (true)
	{
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		for (int j = 0; j < FD_SETSIZE; ++j)
		{
			if (iter == moniter.user_socket.end())
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
			if (iter == moniter.user_socket.end())
				break;
			continue;
		}

		for (UINT i = 0; i < writeSet.fd_count; ++i)
		{
			SendProc(writeSet.fd_array[i]);
		}

		for (UINT i = 0; i < readSet.fd_count; ++i)
		{
			RecvProc(readSet.fd_array[i]);
		}

		// ���� ����
		if (iter == moniter.user_socket.end())
			break;
	}
}

/*----------------------------------------------------------*/
// ::RecvProc
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
bool RecvProc(SOCKET recvSocket)
{
	Session * sendSession = moniter.user_socket[recvSocket];

	// �ѹ��� ���尡���� ���̳� �ִ� 2000����Ʈ
	int size = min(sendSession->recvStream.GetNotBrokenPutSize(), 2000);
	int received = recv(sendSession->socket, sendSession->recvStream.GetWriteBufferPtr(), size, 0);
	// ���� �޽���
	if (received == 0 || received == SOCKET_ERROR)
	{
		LogoutProcess(recvSocket);
		return false;
	}
	sendSession->recvStream.MoveWritePos(received);
	return true;
}

/*----------------------------------------------------------*/
// ::SendProc
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void SendProc(SOCKET sendSocket)
{
	Session * sendSession = moniter.user_socket[sendSocket];
	while (1)
	{
		int sendSize = sendSession->sendStream.GetNotBrokenGetSize();
		// send �����۰� �����
		if (sendSize == 0)
			break;
		// blockSocket�̱� ������ sendSize == retval�� �ƴϸ� �������°��Ǹ� ������ �������� ����
		int retval = send(sendSession->socket, sendSession->sendStream.GetReadBufferPtr(), sendSize, 0);
		if (retval == SOCKET_ERROR)
			break;
		else
			sendSession->sendStream.RemoveData(retval);
	}
}

/*----------------------------------------------------------*/
// ::PacketRecv
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void PacketRecv()
{
	map<SOCKET, Session *>::iterator iter = moniter.user_socket.begin();
	map<SOCKET, Session *>::iterator iterEnd = moniter.user_socket.end();
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
				// ��Ŷ�ڵ� ����
				if (packetHeader.byCode != dfPACKET_CODE)
				{
					CloseSession(iter->second);
					delete iter->second->character;
					delete iter->second;
					iter = moniter.user_socket.erase(iter);
					if (iter == moniter.user_socket.end())
						return;
					--iter;
					break;
				}
			}

			
			// ����� �޾����� payload�� ��� �������� �ʾҴ�.
			if (iter->second->recvStream.GetUseSize() < packetHeader.bySize + sizeof(BYTE))
			{
				iter->second->saveHeaderFlag = true;
				iter->second->saveHeader = packetHeader;
				break;
			}

			StreamBuffer payLoad(packetHeader.bySize);
			received = iter->second->recvStream.Dequeue(payLoad.GetBuffer(), packetHeader.bySize);
			payLoad.moveEndIndex();

			BYTE endCode;
			int retval = iter->second->recvStream.Peek((char *)&endCode, sizeof(endCode));
			if (retval != sizeof(endCode))
			{
				moniter.endCodeError++;
				LogoutProcess(iter->second->socket);
				//LOG(Logger::LOG_LEVEL_ERROR, L"EndCode Error Socket %d\n", iter->second->socket);
			}
			else
			{
				iter->second->recvStream.RemoveData(1);
				if (endCode == dfNETWORK_PACKET_END)
					Parsing(iter->second, packetHeader.byType, &payLoad);
				else
				{
					LogoutProcess(iter->second->socket);
					moniter.endCodeError++;
					//LOG(Logger::LOG_LEVEL_ERROR, L"EndCode Error Socket %d\n", iter->second->socket);
				}
			}
		}
		++iter;
	}
}

/*----------------------------------------------------------*/
// ::LoginProcess
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
//bool LoginProcess(SOCKET socket, SOCKADDR_IN addr)
//{
//	Session * newSession = new Session(socket, addr);
//	Player * newPlayer = new Player(newSession, g_ID);
//	if (newSession == nullptr || newPlayer == nullptr)
//	{
//		//LOG(Logger::LOG_LEVEL_ERROR, L"new operator error");
//		return false;
//	}
//	StreamBuffer * packet;
//	MakePacket_Create_My_Character(&packet, g_ID, ACTION_MOVE_LL, newPlayer->_pos, newPlayer->_healthPoint);
//	UniCast(packet, newSession);
//	// �� ������ �������� �����, ������������ �� ������
//	SECTOR_MANAGER.NewPlayer(newPlayer);
//	g_ID++;
//	return true;
//}

/*----------------------------------------------------------*/
// ::LogoutProcess
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void LogoutProcess(SOCKET socket)
{
	//// ���Ϳ��� ����
	//Session * logout = g_user_socket[socket];
	//SECTOR_MANAGER.DeletePlayer(logout->character);
	//// ���Ǹ���Ʈ���� ����
	//g_user_socket.erase(socket);
	//// �������� ���Ǹ���Ʈ�� ����
	//g_deleteUser_socket.insert(pair<SOCKET, Session *>(socket, logout));

	//// ���ϴݱ�
	//closesocket(socket);
	//// �ش� ���� ������ ������ �������� �˸�
	//Session * logout = g_user_socket[socket];
	//StreamBuffer * packet;
	//MakePacket_Delete_Character(&packet, logout->character->GetID());
	//BroadCast_Sector(packet, logout);

	//LOG(Logger::LOG_LEVEL_DEBUG, L"DisConnect %Id\n", socket);
	//delete logout->character;
	//delete logout;

	closesocket(socket);
	Session * logout = moniter.user_socket[socket];
	moniter.user_socket.erase(socket);

	delete logout->character;
	delete logout;

	moniter.sessionLiveCount--;
	moniter.sessionDeadCount++;
	
}

/*----------------------------------------------------------*/
// ::Parsing
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void Parsing(Session * session, WORD message, StreamBuffer * payload)
{
	session->lastRecvTime = GetTickCount64();
	switch (message)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		int id;
		*payload >> id;
		session->character = new Player(session, id);
		*payload >> session->character->_direction;
		*payload >> session->character->_pos.x;
		*payload >> session->character->_pos.y;
		*payload >> session->character->_healthPoint;
		session->character->_moveStartPos = session->character->_pos;
		moniter.recvCount++;
		break;
	}
	case dfPACKET_SC_MOVE_STOP:
	{
		int id;
		*payload >> id;
		if (id == session->character->_ID)
		{
			*payload >> session->character->_direction;
			*payload >> session->character->_pos.x;
			*payload >> session->character->_pos.y;
		}
		session->character->_moveStartPos = session->character->_pos;
		moniter.recvCount++;
		break;
	}

	case dfPACKET_SC_ATTACK1:
	{
		int id;
		*payload >> id;
		if (id == session->character->_ID)
		{
			*payload >> session->character->_direction;
			*payload >> session->character->_pos.x;
			*payload >> session->character->_pos.y;
			session->character->_moveStartPos = session->character->_pos;
		}
		moniter.recvCount++;
		break;
	}
	case dfPACKET_SC_ATTACK2:
	{
		int id;
		*payload >> id;
		if (id == session->character->_ID)
		{
			*payload >> session->character->_direction;
			*payload >> session->character->_pos.x;
			*payload >> session->character->_pos.y;
			session->character->_moveStartPos = session->character->_pos;
		}
		moniter.recvCount++;
		break;
	}
	case dfPACKET_SC_ATTACK3:
	{
		int id;
		*payload >> id;
		if (id == session->character->_ID)
		{
			*payload >> session->character->_direction;
			*payload >> session->character->_pos.x;
			*payload >> session->character->_pos.y;
			session->character->_moveStartPos = session->character->_pos;
		}
		moniter.recvCount++;
		break;
	}
	case dfPACKET_SC_SYNC:
	{
		int id;
		*payload >> id;
		if (id == session->character->_ID)
		{
			*payload >> session->character->_pos.x;
			*payload >> session->character->_pos.y;
			session->character->_moveStartPos = session->character->_pos;
		}
		moniter.syncCount++;
	}
	default:
		//LOG(Logger::LOG_LEVEL_WARNING, L"wrong packet");
		moniter.recvCount++;
		break;
	}
}

/*----------------------------------------------------------*/
// ::CloseSession
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void CloseSession(Session * session)
{
	closesocket(session->socket);
}

/*----------------------------------------------------------*/
// ::CheckDeadReckoning
// ���� : 
// ���� : 
// ���� :
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
// ���� : 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
//void DeleteSession()
//{
//	map<SOCKET, Session *>::iterator iter = g_deleteUser_socket.begin();
//	map<SOCKET, Session *>::iterator iterEnd = g_deleteUser_socket.end();
//	for (; iter != iterEnd;)
//	{
//		StreamBuffer * deleteUser;
//		MakePacket_Delete_Character(&deleteUser, iter->second->character->_ID);
//		BroadCast_Sector(deleteUser, iter->second);
//		LOG(Logger::LOG_LEVEL_DEBUG, L"DisConnect %Id\n", iter->second->character->_ID);
//		delete iter->second->character;
//		delete iter->second;
//		iter = g_deleteUser_socket.erase(iter);
//	}
//}