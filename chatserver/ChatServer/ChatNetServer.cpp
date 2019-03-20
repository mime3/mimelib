#include "pch.h"


extern bool Check_1Sec(ULONGLONG * time);

/*----------------------------------------------------------*/
// ChatNetServer::UpdateThreadMain (private) (static) (callback)
// ���� : �������� ó���ϴ� �ܵ� Update������
// ���� : (LPVOID) this�� �Ѱܹ���
// ���� : (unsigned int) 0
/*----------------*////////////////////////*----------------*/
unsigned int __stdcall ChatNetServer::UpdateThreadMain(LPVOID lpParam)
{
	ChatNetServer * _this = (ChatNetServer *)lpParam;
	while (true)
	{
		Message * message = nullptr;
		if (!_this->GetMessageData(&message))
		{
			// �����߻�����
			if (_this->_exitFlag)
				break;
			WaitForSingleObject(_this->_wakeUpEvent, INFINITE);
			continue;
		}
		long tps = InterlockedIncrement((LONG*)&_this->_UPDATE_TPS);
		
		switch(message->type)
		{
		case MESSAGE::MESSAGE_TYPE::JOIN:
			_this->NewPlayer(message);
			break;
		case MESSAGE::MESSAGE_TYPE::LEAVE:
			_this->DeletePlayer(message->sessionID);
			break;
		case MESSAGE::MESSAGE_TYPE::PACKET:
			_this->PacketProc(message);
			PacketFree(message->data);
			break;
		default:
			_this->OnError(PACKET_TYPE_ERROR, L"PacketType ERROR");
			break;
		}
		_this->_memoryPoolMessage.Free(message);
		
	}
	return 0;
}

/*----------------------------------------------------------*/
// ChatNetServer::PutMessageData (private)
// ���� : Message�� ť�� �ְ� Update�����带 �����
// ���� : (Message *) �޽��� ������ 
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::PutMessageData(Message * message)
{
	_messageQueue.EnQueue(message);
	SetEvent(_wakeUpEvent);
}

/*----------------------------------------------------------*/
// ChatNetServer::GetMessageData (private)
// ���� : ť���� Message�� ������
// ���� : (Message **) �޽��������͸� ���� �ּ� 
// ���� : (bool) ��������
/*----------------*////////////////////////*----------------*/
bool ChatNetServer::GetMessageData(Message ** message)
{
	return _messageQueue.DeQueue(message);
}

/*----------------------------------------------------------*/
// ChatNetServer::NewPlayer (private)
// ���� : ���ο� �÷��̾� ����
// ���� : (MESSAGE *) �޽��� ������ 
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::NewPlayer(MESSAGE * message)
{
	PLAYER * newPlayer = _memoryPoolPlayer.Alloc();
	newPlayer->sessionID = message->sessionID;
	newPlayer->loginFlag = false;
	newPlayer->accountNo = 0;
	// .. sector�� ����?
	newPlayer->loginFlag = false;
	newPlayer->sectorX = -1;
	newPlayer->sectorY = -1;
	_playerMap.insert(pair<__int64, PLAYER *>(newPlayer->sessionID, newPlayer));
}

/*----------------------------------------------------------*/
// ChatNetServer::DeletePlayer (private)
// ���� : �÷��̾� ����
// ���� : (__int64)  ����ID
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::DeletePlayer(__int64 sessionID)
{
	auto find = _playerMap.find(sessionID);
	if (find == _playerMap.end())
		return;
	PLAYER * player = find->second;
	if(player->loginFlag)
		InterlockedDecrement((LONG *)&_LOGIN_PLAYER);
	_playerMap.erase(sessionID);

	// ���Ϳ��� �÷��̾� ����
	if (!(player->sectorX < 0) && !(SECTOR_X - 1 < player->sectorX))
	{
		if (!(player->sectorY < 0) && !(SECTOR_Y - 1 < player->sectorY))
		{
			auto iter = _sector[player->sectorY][player->sectorX].playerSet.begin();
			auto iterEnd = _sector[player->sectorY][player->sectorX].playerSet.end();
			for (; iter != iterEnd; ++iter)
			{
				if (*iter == sessionID)
				{
					_sector[player->sectorY][player->sectorX].playerSet.erase(iter);
					break;
				}
			}
		}
	}
	_memoryPoolPlayer.Free(player);
}

/*----------------------------------------------------------*/
// ChatNetServer::PacketProc (private)
// ���� : ��Ŷ�� ó���ϱ����� type���� �Ľ��ϴºκ�
// ���� : (MESSAGE *) �޽���
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::PacketProc(MESSAGE * message)
{
	WORD type;
	*(message->data) >> type;
	switch (type)
	{
	case en_PACKET_CS_CHAT_REQ_LOGIN:
		Login(message->sessionID, message->data);
		break;
	case en_PACKET_CS_CHAT_REQ_SECTOR_MOVE:
		MoveSector(message->sessionID, message->data);
		break;
	case en_PACKET_CS_CHAT_REQ_MESSAGE:
		Chat(message->sessionID, message->data);
		break;
	case en_PACKET_CS_CHAT_REQ_HEARTBEAT:
		break;
	default:
		OnError(PACKET_TYPE_ERROR, L"Packet Type ERROR");
		break;
	}
}

/*----------------------------------------------------------*/
// ChatNetServer::Login (private)
// ���� : �α��� ��Ŷ ó����
// ���� : (__int64) ����ID, (StreamBuffer *) type�� ���� ��Ŷ
// ���� : (bool) ���� ó������
/*----------------*////////////////////////*----------------*/
bool ChatNetServer::Login(__int64 sessionID, StreamBuffer * data)
{
	auto find = _playerMap.find(sessionID);
	if (find == _playerMap.end())
	{
		OnError(NOT_EXIST_PLAYER, L"Not_Exist_Player");
		return false;
	}
	PLAYER * player = find->second;

	*data >> player->accountNo;
	data->Out((char *)player->ID, 20 * 2);
	data->Out((char *)player->nickName, 20 * 2);
	data->Out(player->sessionKey, 64);

	 //����
	AcquireSRWLockExclusive(_keyMapLock);
	auto iter = _keyMap->find(player->accountNo);
	if (iter == _keyMap->end())
	{
		 OnError(NOT_EXIST_SESSIONKEY, L"Not_Exist_SessionKey");
		 InterlockedIncrement((LONG *)&_NOT_EXIST_SESSIONKEY);
		 ReleaseSRWLockExclusive(_keyMapLock);
		 return false;
	}

	if(memcmp(iter->second.key, player->sessionKey, 64) != 0)
	{
		 OnError(WRONG_SESSIONKEY, L"Wrong_SessionKey");
		 InterlockedIncrement((LONG *)&_WRONG_SESSIONKEY);
		 ReleaseSRWLockExclusive(_keyMapLock);
		 return false;
	}

	_keyMap->erase(iter);
	ReleaseSRWLockExclusive(_keyMapLock);

	player->sectorX = -1;
	player->sectorY = -1;
	player->loginFlag = true;

	InterlockedIncrement((LONG *)&_LOGIN_PLAYER);

	StreamBuffer * Login_Res = PacketAlloc(NetServer);
	MakePacket_Res_Login(Login_Res, true, player->accountNo);
	SendPacket(sessionID, Login_Res);
	PacketFree(Login_Res);
	return true;
}

/*----------------------------------------------------------*/
// ChatNetServer::MoveSector (private)
// ���� : �����̵� ��Ŷ ó����
// ���� : (__int64) ����ID, (StreamBuffer *) type�� ���� ��Ŷ
// ���� : (bool) ���� ó������
/*----------------*////////////////////////*----------------*/
bool ChatNetServer::MoveSector(__int64 sessionID, StreamBuffer * data)
{
	auto find = _playerMap.find(sessionID);
	if (find == _playerMap.end())
	{
		OnError(NOT_EXIST_PLAYER, L"Not_Exist_Player");
		return false;
	}
	PLAYER * player = find->second;

	if (player->loginFlag == false)
	{
		OnError(NOT_LOGIN, L"Not_Login_Player");
		return false;
	}

	__int64 accountNo;
	WORD sectorX, sectorY;
	*data >> accountNo >> sectorX >> sectorY;

	if (player->accountNo != accountNo)
	{
		OnError(WRONG_ACCOUNT_NO, L"Wrong_Account_No");
		return false;
	}
	if (sectorX < 0 || SECTOR_X - 1 < sectorX)
	{
		OnError(WRONG_MOVE_SECTOR_X, L"Wrong_Move_Sector_X");
		return false;
	}
	if (sectorY < 0 || SECTOR_Y - 1 < sectorY)
	{
		OnError(WRONG_MOVE_SECTOR_Y, L"Wrong_Move_Sector_Y");
		return false;
	}
	
	// �α��� ���Ĵ� -1�̶� �˻�
	if (player->sectorX != 65535)
	{
		auto iter = _sector[player->sectorY][player->sectorX].playerSet.begin();
		auto iterEnd = _sector[player->sectorY][player->sectorX].playerSet.end();
		for (; iter != iterEnd; ++iter)
		{
			if (*iter == sessionID)
			{
				_sector[player->sectorY][player->sectorX].playerSet.erase(iter);
				break;
			}
		}
	}

	// ���� �̵�
	player->sectorX = sectorX;
	player->sectorY = sectorY;
	_sector[sectorY][sectorX].playerSet.insert(sessionID);
	//_sector[sectorY][sectorX].playerSet.push_back(sessionID);

	StreamBuffer * MoveSector_Res = PacketAlloc(NetServer);
	MakePacket_Res_MoveSector(MoveSector_Res, player->accountNo, sectorX, sectorY);
	SendPacket(sessionID, MoveSector_Res);
	PacketFree(MoveSector_Res);
	return true;
}

/*----------------------------------------------------------*/
// ChatNetServer::Chat (private)
// ���� : ä�� ��Ŷ ó����
// ���� : (__int64) ����ID, (StreamBuffer *) type�� ���� ��Ŷ 
// ���� : (bool) ���� ó������
/*----------------*////////////////////////*----------------*/
bool ChatNetServer::Chat(__int64 sessionID, StreamBuffer * data)
{
	// player�˻�
	auto find = _playerMap.find(sessionID);
	if (find == _playerMap.end())
	{
		OnError(NOT_EXIST_PLAYER, L"Not_Exist_Player");
		return false;
	}
	PLAYER * player = find->second;

	if (player->loginFlag == false)
	{
		OnError(NOT_LOGIN, L"Not_Login_Player");
		return false;
	}

	// data ������
	__int64 accountNo;
	WORD chatLen;
	*data >> accountNo >> chatLen;
	// No����ġ
	if (player->accountNo != accountNo)
	{
		OnError(WRONG_ACCOUNT_NO, L"Wrong_Account_No");
		return false;
	}
	// ChatLen�� ���̰� ���� ��Ŷ�����Ͷ� ����ġ
	if (data->GetUseSize() != chatLen)
	{
		chatLen = data->GetUseSize();
		OnError(WRONG_CHATLEN, L"ChatLen Wrong");
		return false;
	}
	WCHAR * chat = new WCHAR[chatLen / 2 + 1];
	data->Out((char *)chat, chatLen);
	chat[chatLen / 2] = 0;

	// DB�� ����
	//_dbConnector.Query_Save(L"INSERT INTO `chatlog`(`accountno`, `date`, `log`) VALUES(%d, NOW(), \"%s\")",
	//	accountNo, chat);

	// ��Ŷ ����
	StreamBuffer * chat_Res = PacketAlloc(NetServer);
	MakePacket_Res_Chat(chat_Res, player->accountNo, player->ID, player->nickName, chatLen, chat);
	BroadCastNeighborSector(chat_Res, player);
	//BroadCastSector(chat_Res, player->sectorX, player->sectorY);
	//SendPacket(sessionID, chat_Res);
	PacketFree(chat_Res);
	delete[] chat;
	return true;
}

/*----------------------------------------------------------*/
// ChatNetServer::MakePacket_Res_Login (private)
// ���� : �α��� ��Ŷ ����
// ���� : (StreamBuffer *) ��Ŷ������, (bool) �α��� ����, (__int64) ������ȣ
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::MakePacket_Res_Login(StreamBuffer * packet, bool login, __int64 accountNo)
{
	*packet << (WORD)en_PACKET_CS_CHAT_RES_LOGIN << (BYTE)login << accountNo;
}

/*----------------------------------------------------------*/
// ChatNetServer::MakePacket_Res_MoveSector (private)
// ���� : �����̵� ��Ŷ ����
// ���� : (StreamBuffer *) ��Ŷ������, (__int64) ������ȣ, (WORD)���� X��ǥ, (WORD) ���� Y��ǥ
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::MakePacket_Res_MoveSector(StreamBuffer * packet, __int64 accountNo, WORD sectorX, WORD sectorY)
{
	*packet << (WORD)en_PACKET_CS_CHAT_RES_SECTOR_MOVE << accountNo << sectorX << sectorY;
}

/*----------------------------------------------------------*/
// ChatNetServer::MakePacket_Res_Chat (private)
// ���� : ä�� ��Ŷ ����
// ���� : (StreamBuffer *) ��Ŷ������, (__int64) ������ȣ, (WCHAR) ID ������, (WCHAR) �г��� ������, (WORD) ä�� ����Ʈ����, (WCHAR) ä�� ������ 
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::MakePacket_Res_Chat(StreamBuffer * packet, __int64 accountNo, WCHAR * ID, WCHAR * nickName, WORD chatLen, WCHAR * chat)
{
	*packet << (WORD)en_PACKET_CS_CHAT_RES_MESSAGE << accountNo;
	packet->In((char *)ID, 20 * 2);
	packet->In((char *)nickName, 20 * 2);
	*packet << chatLen;
	packet->In((char *)chat, chatLen);
}

/*----------------------------------------------------------*/
// ChatNetServer::BroadCastNeighborSector (private)
// ���� : �÷��̾��� ���� ��ġ�� ���Ϳ� �� �ֺ������� �÷��̾�� ��ε�ĳ��Ʈ
// ���� : (StreamBuffer *) ��Ŷ ������, (PLAYER *) �÷��̾� ������
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::BroadCastNeighborSector(StreamBuffer * packet, PLAYER * player)
{
	// 1���;� ��ε�ĳ��Ʈ, �� 9����
	WORD sectorX = player->sectorX;
	WORD sectorY = player->sectorY;
	BroadCastSector(packet, sectorX, sectorY);
	BroadCastSector(packet, sectorX, sectorY + 1);
	BroadCastSector(packet, sectorX, sectorY - 1);
	BroadCastSector(packet, sectorX + 1, sectorY);
	BroadCastSector(packet, sectorX + 1, sectorY + 1);
	BroadCastSector(packet, sectorX + 1, sectorY - 1);
	BroadCastSector(packet, sectorX - 1, sectorY);
	BroadCastSector(packet, sectorX - 1, sectorY + 1);
	BroadCastSector(packet, sectorX - 1, sectorY - 1);
}

/*----------------------------------------------------------*/
// ChatNetServer::BroadCastSector (private)
// ���� : ���;��� �÷��̾�� ��ε�ĳ��Ʈ
// ���� : (StreamBuffer *) ��Ŷ ������, (WORD) ���� X��ǥ, (WORD) ���� Y��ǥ
// ���� : 
/*----------------*////////////////////////*----------------*/
void ChatNetServer::BroadCastSector(StreamBuffer * packet, WORD sectorX, WORD sectorY)
{
	// ������ġ Ȯ��
	if (sectorX < 0 || SECTOR_X - 1 < sectorX)
		return;
	if (sectorY < 0 || SECTOR_Y - 1 < sectorY)
		return;

	auto iter = _sector[sectorY][sectorX].playerSet.begin();
	auto iterEnd = _sector[sectorY][sectorX].playerSet.end();
	for (; iter != iterEnd; ++iter)
	{
		SendPacket(*iter, packet);
	}
}

/*----------------------------------------------------------*/
// ChatNetServer::ChatNetServer (public)
// ���� : ������
// ���� :  
// ���� : 
/*----------------*////////////////////////*----------------*/
ChatNetServer::ChatNetServer() : _dbConnector("DBconfig.cnf")
{
	_exitFlag = false;
	_wakeUpEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	_updateThread = (HANDLE)_beginthreadex(NULL, 0, UpdateThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
}

/*----------------------------------------------------------*/
// ChatNetServer::~ChatNetServer (public)
// ���� : �Ҹ���
// ���� :  
// ���� : 
/*----------------*////////////////////////*----------------*/
ChatNetServer::~ChatNetServer()
{
	if (_exitFlag != true)
		Stop();
}

/*----------------------------------------------------------*/
// ChatNetServer::~ChatNetServer (public)
// ���� : �Ҹ���
// ���� :  
// ���� : 
/*----------------*////////////////////////*----------------*/

void ChatNetServer::Stop()
{
	NetServer::Stop();

	// ������Ʈ ������ ����
	_exitFlag = true;
	SetEvent(_wakeUpEvent);
	WaitForSingleObject(_updateThread, INFINITE);
	// ��� �÷��̾� �����
	auto iter = _playerMap.begin();
	auto iterEnd = _playerMap.end();
	for (; iter != iterEnd;)
	{
		_memoryPoolPlayer.Free(iter->second);
		iter = _playerMap.erase(iter);
	}
	// ��� �޽��� �����
	INT64 qsize = _messageQueue.GetUseCount();
	while (qsize--)
	{
		Message * message = nullptr;
		if (!_messageQueue.DeQueue(&message))
			break;
		_memoryPoolMessage.Free(message);
	}
	CloseHandle(_wakeUpEvent);
	CloseHandle(_updateThread);
}

/*----------------------------------------------------------*/
// ChatNetServer::OnClientJoin ()
// ���� : 
// ���� : () 
// ���� : ()
/*----------------*////////////////////////*----------------*/
void ChatNetServer::OnClientJoin(__int64 ClientID, Session * session)
{
	if (_exitFlag)
		return;
	MESSAGE * message = _memoryPoolMessage.Alloc();
	message->type = MESSAGE::MESSAGE_TYPE::JOIN;
	message->sessionID = ClientID;
	PutMessageData(message);
}

/*----------------------------------------------------------*/
// ChatNetServer::OnClientLeave ()
// ���� : 
// ���� : () 
// ���� : ()
/*----------------*////////////////////////*----------------*/
void ChatNetServer::OnClientLeave(__int64 ClientID)
{
	if (_exitFlag)
		return;
	MESSAGE * message = _memoryPoolMessage.Alloc();
	message->type = MESSAGE::MESSAGE_TYPE::LEAVE;
	message->sessionID = ClientID;
	PutMessageData(message);
}

/*----------------------------------------------------------*/
// ChatNetServer::OnConnectionRequest ()
// ���� : 
// ���� : () 
// ���� : ()
/*----------------*////////////////////////*----------------*/
bool ChatNetServer::OnConnectionRequest(WCHAR * ClientIP, int Port)
{
	return true;
}

/*----------------------------------------------------------*/
// ChatNetServer::OnRecv ()
// ���� : 
// ���� : () 
// ���� : ()
/*----------------*////////////////////////*----------------*/
void ChatNetServer::OnRecv(__int64 clientID, StreamBuffer * packet)
{
	if (_exitFlag)
		return;

	StreamBuffer * messageData = PacketAlloc(NetServer);
	memcpy_s(messageData->GetBuffer(), messageData->GetSize(), packet->GetBuffer(), packet->GetUseSize());
	messageData->MoveEndIndex(packet->GetUseSize());

	MESSAGE * message = _memoryPoolMessage.Alloc();
	message->type = MESSAGE::MESSAGE_TYPE::PACKET;
	message->sessionID = clientID;
	message->data = messageData;
	PutMessageData(message);
}

/*----------------------------------------------------------*/
// ChatNetServer::OnSend ()
// ���� : 
// ���� : () 
// ���� : ()
/*----------------*////////////////////////*----------------*/
void ChatNetServer::OnSend(__int64 clientID, int sendSize)
{
}

/*----------------------------------------------------------*/
// ChatNetServer::OnError ()
// ���� : 
// ���� : () 
// ���� : ()
/*----------------*////////////////////////*----------------*/
void ChatNetServer::OnError(int errorCode, const WCHAR * errorMessage)
{
	LOG(L"Server_OnError", Logger::LOG_LEVEL_WARNING, errorMessage);
}
