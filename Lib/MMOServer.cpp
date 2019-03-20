#include "pch.h"
#include "MMOServer.h"

extern CrashDump cd;

bool MMOServer::LoadConfig(const char * fileName)
{
	if(!_configParser.OpenFile(fileName))
		return false;

	_configParser.GetValue_Str("BIND_IP", _bindIP, "SERVER");
	_configParser.GetValue_Int("BIND_PORT", _serverPort, "SERVER");
	_configParser.GetValue_Int("WORKER_THREAD", _workerThreadCount, "SERVER");
	_configParser.GetValue_Int("ACTIVE_WORKER_THREAD", _activeWorkerThreadCount, "SERVER");
	_configParser.GetValue_Int("SEND_THREAD", _sendThreadCount, "SERVER");
	_configParser.GetValue_Int("CLIENT_MAX", _maxClient, "SERVER");
	_configParser.GetValue_Int("MONITOR_NO", _monitorNo, "SERVER");

	_configParser.GetValue_BYTE("PACKET_CODE", _packetCode, "SERVER");
	_configParser.GetValue_BYTE("PACKET_CODE", _XORKey1, "SERVER");
	_configParser.GetValue_BYTE("PACKET_CODE", _XORKey2, "SERVER");

	_configParser.GetValue_Int("ACCEPT_SLEEP", ACCEPT_SLEEP, "SERVER");
	_configParser.GetValue_Int("SEND_SLEEP", SEND_SLEEP, "SERVER");
	_configParser.GetValue_Int("AUTH_SLEEP", AUTH_SLEEP, "SERVER");
	_configParser.GetValue_Int("CONTENTS_SLEEP", CONTENTS_SLEEP, "SERVER");
	_configParser.GetValue_Int("RELEASE_SLEEP", RELEASE_SLEEP, "SERVER");
	_configParser.GetValue_Int("MAX_AUTH_COUNT", MAX_AUTH_COUNT, "SERVER");
	_configParser.GetValue_Int("MAX_AUTH_TO_CONTENTS", MAX_AUTH_TO_CONTENTS, "SERVER");
	_configParser.GetValue_Int("AUTH_PACKET_PROCESS", AUTH_PACKET_PROCESS, "SERVER");
	_configParser.GetValue_Int("CONTENTS_PACKET_PROCESS", CONTENTS_PACKET_PROCESS, "SERVER");

	char loglevel[10];
	_configParser.GetValue_Str("LOG_LEVEL", loglevel, "SERVER");
	if (strcmp(loglevel, "DEBUG") == 0)
		LOGGER.SetLogLevel(LOG_DEBUG);
	else if (strcmp(loglevel, "WARNING") == 0)
		LOGGER.SetLogLevel(LOG_WARNING);
	else if (strcmp(loglevel, "ERROR") == 0)
		LOGGER.SetLogLevel(LOG_ERROR);
	else
		LOGGER.SetLogLevel(LOG_DEBUG);

	_configParser.CloseFile();
	return true;
}

void MMOServer::InitListenSocket()
{
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	char reUseAddr = 1;
	setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &reUseAddr, sizeof(reUseAddr));
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	inet_pton(AF_INET, _bindIP, &addr.sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_serverPort);
	bind(_listenSocket, (SOCKADDR*)&addr, sizeof(addr));
	listen(_listenSocket, SOMAXCONN);
}

void MMOServer::RecvProc(MMOSession * session)
{
	// 완성패킷 저장
	BYTE packetCode = _packetCode;
	while (1)
	{
		PACKET_HEADER header;
		int headerSize = sizeof(header);
		int useSize = session->_recvQueue.GetUseSize();
		if (useSize < headerSize)
			break;
		int ret = session->_recvQueue.Peek((char *)&header, headerSize);

		if (header.code != packetCode)
		{
			LOG(L"Server", LOG_ERROR, L"Packet Code is Wrong");
			shutdown(session->_socket, SD_SEND);
			break;
		}

		if (PacketSIZE < header.len)
		{
			LOG(L"Server", LOG_ERROR, L"Packet Len is Over %d", PacketSIZE);
			shutdown(session->_socket, SD_SEND);
			break;
		}

		if (useSize < header.len + headerSize)
			break;
		session->_recvQueue.RemoveData(headerSize);
		// 패킷을 하나씩 꺼낸다
		StreamBuffer * packet = PacketAlloc(MMOServer);
		ret = session->_recvQueue.Dequeue(packet->GetBuffer(), header.len);
		packet->MoveEndIndex(header.len);
		InterlockedIncrement(&_recvTPS);
		if (Decode(&header, packet))
			session->_completeRecvQueue.EnQueue_UnSafe(packet);
			//session->_completeRecvQueue.push(packet);
		else
			PacketFree(packet);
	}
}

inline BYTE MMOServer::GetCheckSum(char * buffer, int size)
{
	BYTE checkSum = 0;
	while (size--)
	{
		checkSum += *(buffer++);
	}
	return checkSum;
}

inline void MMOServer::XOR(char * buffer, int size, char key)
{
	while (size--)
	{
		*(buffer++) ^= key;
	}
}

void MMOServer::Encode(StreamBuffer * packet)
{
	PACKET_HEADER * bufferPointer = (PACKET_HEADER *)packet->GetBuffer();

	XOR((char *)&bufferPointer->checkSum, bufferPointer->len + 1, bufferPointer->randXORKey);
	XOR((char *)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey1);
	XOR((char *)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey2);
}

bool MMOServer::Decode(PACKET_HEADER * header, StreamBuffer * payloadPacket)
{
	char randKey = header->randXORKey;
	char key1 = _XORKey1;
	char key2 = _XORKey2;
	char * payloadPointer = payloadPacket->GetBuffer();
	int payloadSize = payloadPacket->GetUseSize();

	//XOR(payloadPointer, payloadSize, key2);
	//XOR((char *)&(header->randXORKey), 2, key2);

	//XOR(payloadPointer, payloadSize, key1);
	//XOR((char *)&(header->randXORKey), 2, key1);

	XOR(payloadPointer, payloadSize, /*key1 ^ key2 ^*/ randKey);
	//XOR((char *)&(header->randXORKey), 2, key1 ^ key2);

	//XOR(payloadPointer, payloadSize, header->randXORKey);
	XOR((char *)&header->checkSum, 1, randKey);

	BYTE checkSum = GetCheckSum(payloadPointer, payloadSize);
	if (checkSum != header->checkSum)
	{
		LOG(L"Server", LOG_ERROR, L"Decode CheckSum Error");
		return false;
	}
	return true;
}

bool MMOServer::RecvPost(MMOSession * session)
{
	//리시브 큐가 꽉 찼는지 체크
	int fullSize = session->_recvQueue.GetBufferSize();
	int freeSize = session->_recvQueue.GetFreeSize();
	// 리시브큐 5% 미만 남음
	if (fullSize / 100 * 95 > freeSize)
	{
		// 종료
		LOG(L"Server", LOG_ERROR, L"SessionID : %d , RecvQueue Full", session->_sessionID);
		shutdown(session->_socket, SD_SEND);
		return false;
	}
	// wsabuf 세팅
	WSABUF wsaBuf[2];
	wsaBuf[0].buf = session->_recvQueue.GetWriteBufferPtr();
	wsaBuf[0].len = session->_recvQueue.GetNotBrokenPutSize();
	wsaBuf[1].buf = session->_recvQueue.GetBufferPtr();
	wsaBuf[1].len = freeSize - wsaBuf[0].len;
	//overlapped 초기화
	ZeroMemory(&session->_recvOverLapped, sizeof(OVERLAPPED));
	//iocount++
	InterlockedIncrement((LONG *)&session->_ioCount);
	//wsaRecv()
	DWORD transBytes = 0;
	DWORD flags = 0;
	int check = 0;
	(freeSize == wsaBuf[0].len) ? (check = 1) : (check = 2);
	int retval = WSARecv(session->_socket, wsaBuf, 2, &transBytes, &flags, &session->_recvOverLapped, NULL);
	//에러처리
	if (retval == SOCKET_ERROR)
	{
		int errorCode = GetLastError();
		if (errorCode != ERROR_IO_PENDING)
		{
			if (errorCode == WSAENOBUFS)
				LOG(L"Server", LOG_ERROR, L"RecvPost Fail WSAENOBUFS, Socket : %d", session->_socket);
			else if (errorCode == WSAECONNRESET)
				;
			else
			{
				LOG(L"Server", LOG_ERROR, L"RecvPost Fail %d, Socket : %d", errorCode, session->_socket);
			}
			// 수신 등록 실패, 종료코드
			// LOG
			int ioCount = InterlockedDecrement((LONG *)&session->_ioCount);
			shutdown(session->_socket, SD_SEND);
			if (ioCount == 0)
				session->_logOutFlag = true;
			return false;
		}
	}
	return true;
}

unsigned int __stdcall MMOServer::AcceptThreadMain(LPVOID param)
{
	MMOServer * _this = (MMOServer *)param;
	_this->InitListenSocket();
	while (1)
	{
		SOCKADDR_IN clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		SOCKET client = accept(_this->_listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
		InterlockedIncrement(&_this->_acceptTPS);
		_this->_acceptCount++;

		DWORD opt;
		setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt));
		
		linger ling;
		ling.l_onoff = 1;
		ling.l_linger = 0;
		setsockopt(client, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

		if (client == INVALID_SOCKET)
		{
			int errorCode = GetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				Sleep(1000);
				continue;
			}
			// accept중 리슨소켓을 닫아서 이쪽으로 유도
			else if (errorCode == WSAEINTR)
				break;
			else if (errorCode == WSAENOTSOCK)
				break;
			// 뭔가 에러인부분
			LOG(L"Server", LOG_ERROR, L"Accept Invaild Socket");
			continue;
		}
		ACCEPT_INFO * acceptInfo = _this->_acceptInfoPool.Alloc();
		acceptInfo->socket = client;
		InetNtop(AF_INET, &clientAddr.sin_addr, acceptInfo->IP, 16);
		acceptInfo->port = clientAddr.sin_port;
		_this->_acceptQueue.EnQueue_UnSafe(acceptInfo);
	}
	return 0;
}

unsigned int __stdcall MMOServer::WorkerThreadMain(LPVOID param)
{
	MMOServer * _this = (MMOServer*)param;
	while (1)
	{
		DWORD transBtyes = 0;
		LPOVERLAPPED overlapped = nullptr;
		MMOSession * session = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_this->_iocp, &transBtyes, (PULONG_PTR)&session, &overlapped, INFINITE);
		if (ret && session == NULL)
		{
			//쓰레드 종료
			break;
		}
		if (overlapped == nullptr)
		{
			// 비정상 연결종료시작
			int errorCode = GetLastError();
			shutdown(session->_socket, SD_SEND);
			//_this->DisConnect(session);
		}
		else
		{
			// 수신완료
			if (&session->_recvOverLapped == overlapped)
			{
				// 정상적 종료 상황
				if (transBtyes == 0)
				{
					//printf("정상종료 메시지 수신\n");
					if (!ret)
					{
						int error = GetLastError();
						if(error != ERROR_NETNAME_DELETED)
							LOG(L"Server", LOG_ERROR, L"recv GQCS FALSE, code = %d", error);
					}
					shutdown(session->_socket, SD_SEND);
				}
				// 정상 수신완료
				else
				{
					session->_recvQueue.MoveWritePos(transBtyes);
					_this->RecvProc(session);
					_this->RecvPost(session);
				}
			}
			// 송신완료
			else if (&session->_sendOverLapped == overlapped)
			{
				if (transBtyes == 0)
				{
					//printf("정상종료 메시지 수신\n");
					if (!ret)
					{
						int error = GetLastError();
						LOG(L"Server", LOG_ERROR, L"send GQCS FALSE, code = %d", error);
					}
					shutdown(session->_socket, SD_SEND);
					session->_sendCount = 0;
					session->_sendFlag = FALSE;
				}
				else
				{
					int count = session->_sendCount;
					InterlockedAdd(&_this->_sendTPS, count);
					for (int i = 0; i < count; i++)
					{
						PacketFree(session->_sendArray[i]);
					}
					session->_sendCount = 0;
					session->_sendFlag = FALSE;
					ZeroMemory(session->_sendArray, sizeof(session->_sendArray));
				}
			}
			// 알수없는 상황
			else
				;
			// 여기서 io 카운트 감소
			int ioCount = InterlockedDecrement((LONG *)&session->_ioCount);
			// 연결종료 체크 구간
			if (ioCount == 0)
				session->_logOutFlag = true;
		}
	}
	return 0;
}

unsigned int __stdcall MMOServer::AuthThreadMain(LPVOID param)
{
	MMOServer * _this = (MMOServer*)param;
	MMOSession ** sessionArray = _this->_sessionArray;
	MMOSession * session = nullptr;
	while (1)
	{
		InterlockedIncrement(&_this->_authFPS);
		int acceptLimit = _this->MAX_AUTH_COUNT;
		// Accpet처리
		while (acceptLimit--)
		{
			ACCEPT_INFO * acceptInfo = nullptr;
			if (_this->_acceptQueue.GetUseCount() == 0)
				break;

			bool ret = _this->_acceptQueue.DeQueue_UnSafe(&acceptInfo);
			if (!ret || acceptInfo == nullptr)
				break;

			int index;
			ret = _this->_indexStack.Pop(&index);
			if (!ret)
			{
				closesocket(acceptInfo->socket);
				_this->_acceptInfoPool.Free(acceptInfo);
				_this->_noIndex++;
				break;
			}

			INT64 sessionID = (_this->_clientSeed++ << 2 * 8) | index;
			_this->_sessionArray[index]->Init(acceptInfo, sessionID);
			_this->_acceptInfoPool.Free(acceptInfo);
			CreateIoCompletionPort((HANDLE)acceptInfo->socket, _this->_iocp, (ULONG_PTR)_this->_sessionArray[index], 0);
			_this->RecvPost(_this->_sessionArray[index]);
			int ioCount = InterlockedDecrement((LONG*)&_this->_sessionArray[index]->_ioCount);
			if (ioCount == 0)
				_this->_sessionArray[index]->_logOutFlag = true;
			InterlockedIncrement(&_this->_AuthSession);
			InterlockedIncrement(&_this->_TotalSession);
		}

		// 패킷처리
		for (int i = 0; i < _this->_maxClient; i++)
		{
			session = sessionArray[i];
			if (session->_mode != AUTH)
				continue;

			// 종료 1단계
			if (session->_logOutFlag == true)
			{
				if(session->_sendFlag == TRUE)
					session->_mode = LOGOUT_IN_AUTH;
				else
				{
					session->_mode = WAIT_LOGOUT;
					InterlockedDecrement(&_this->_AuthSession);
				}
			}
			// 패킷처리
			else
			{
				//int size = (int)session->_completeRecvQueue.size();
				int size = (int)session->_completeRecvQueue.GetUseCount();
				if (size == 0)
					continue;

				int process = _this->AUTH_PACKET_PROCESS;
				while (process--)
				{
					StreamBuffer * packet = nullptr;
					session->_completeRecvQueue.DeQueue_UnSafe(&packet);
					//packet = session->_completeRecvQueue.front();
					if (packet == nullptr)
						continue;
					//session->_completeRecvQueue.pop();
					session->OnAuth_Packet(packet);
					PacketFree(packet);
				}
			}
		}

		// update처리 
		_this->OnAuth_Update();
		
		//// 종료 1단계, 패킷처리중 가능
		//for (int i = 0; i < _this->_maxClient; i++)
		//{
		//	if (_this->_sessionArray[i]->_mode != AUTH)
		//		continue;

		//	if (_this->_sessionArray[i]->_logOutFlag == true)
		//	{
		//		_this->_sessionArray[i]->_mode = LOGOUT_IN_AUTH;
		//	}
		//}

		// 종료 2단계
		for (int i = 0; i < _this->_maxClient; i++)
		{
			session = sessionArray[i];
			// 컨텐츠모드 전환
			if (session->_mode == AUTH)
			{
				if (session->_authToContentsFlag == false)
					continue;

				session->_mode = AUTH_TO_CONTENTS;
				session->OnAuth_ClientLeave(false);
			}
		}

		////컨텐츠모드 전환, 종료2단계중 가능
		//for (int i = 0; i < _this->_maxClient; i++)
		//{
		//	if (_this->_sessionArray[i]->_mode != AUTH)
		//		continue;

		//	if (_this->_sessionArray[i]->_authToContentsFlag == false)
		//		continue;

		//	_this->_sessionArray[i]->_mode = AUTH_TO_CONTENTS;
		//	_this->_sessionArray[i]->OnAuth_ClientLeave(false);
		//}

		Sleep(_this->AUTH_SLEEP);
	}
	return 0;
}

unsigned int __stdcall MMOServer::SendThreadMain(LPVOID param)
{
	MMOServer * _this = (MMOServer*)param;
	int seed = InterlockedIncrement((LONG *)&_this->_sendThreadSeed) - 1;
	int jump = _this->_sendThreadCount;
	MMOSession ** sessionArray = _this->_sessionArray;
	MMOSession * session = nullptr;
	while (1)
	{
		for (int i = seed; i < _this->_maxClient; i += jump)
		{
			session = sessionArray[i];
			if (session->_mode == NONE)
				continue;

			if (session->_logOutFlag == true)
				continue;
			
			if (session->_mode != AUTH && session->_mode != AUTH_TO_CONTENTS && session->_mode != CONTENTS)
				continue;
			
			int useSize = (int)session->_sendQueue.GetUseCount();
			if (useSize == 0)
				continue;
			
			if (session->_sendFlag != FALSE)
				continue;
			

			session->_sendFlag = TRUE;

			WSABUF wsaBuf[mmoSendArraySize];
			ZeroMemory(wsaBuf, sizeof(wsaBuf));
			session->_sendCount = min(useSize, mmoSendArraySize);
			for (int i = 0; i < session->_sendCount; i++)
			{
				StreamBuffer ** packet = &session->_sendArray[i];
				session->_sendQueue.DeQueue_UnSafe(packet);
				wsaBuf[i].buf = (*packet)->GetBuffer();
				wsaBuf[i].len = (*packet)->GetUseSize();
			}
			ZeroMemory(&session->_sendOverLapped, sizeof(OVERLAPPED));
			DWORD transBytes = 0;
			DWORD flags = 0;
			InterlockedIncrement((LONG *)&session->_ioCount);
			int retval = WSASend(session->_socket, wsaBuf, session->_sendCount, &transBytes, flags, &session->_sendOverLapped, NULL);
			if (retval == SOCKET_ERROR)
			{
				int errorCode = GetLastError();
				if (errorCode != ERROR_IO_PENDING)
				{
					if (errorCode == WSAENOBUFS)
						LOG(L"Server", LOG_ERROR, L"SessionID : %d , WSAENOBUFS", session->_sessionID);
					else if (errorCode == WSAECONNRESET)
						;
					else if (errorCode == WSAESHUTDOWN)
						LOG(L"Server", LOG_WARNING, L"SessionID : %d , WSAESHUTDOWN", session->_sessionID);
					else
					{
						LOG(L"Server", LOG_ERROR, L"SessionID : %d , SEND_ERROR %d", session->_sessionID, errorCode);
					}
					int ioCount = InterlockedDecrement((LONG *)&session->_ioCount);
					shutdown(session->_socket, SD_SEND);
					for (int i = 0; i < session->_sendCount; i++)
					{
						PacketFree(session->_sendArray[i]);
					}
					session->_sendFlag = FALSE;
					session->_sendCount = 0;
					if (ioCount == 0)
						session->_logOutFlag = true;
				}
			}
		}
		Sleep(_this->SEND_SLEEP);
	}
	return 0;
}

unsigned int __stdcall MMOServer::ContentsThreadMain(LPVOID param)
{
	MMOServer * _this = (MMOServer*)param;
	MMOSession ** sessionArray = _this->_sessionArray;
	MMOSession * session = nullptr;
	while (1)
	{
		BEGIN("THREAD_MAIN");
		InterlockedIncrement(&_this->_contentsFPS);
		// auth to contents
		int limit = _this->MAX_AUTH_TO_CONTENTS;
		for (int i = 0; i < _this->_maxClient; i++)
		{
			if (limit < 0)
				break;
			session = sessionArray[i];
			if (session->_mode != AUTH_TO_CONTENTS)
				continue;

			session->_mode = CONTENTS;
			session->OnContents_ClientJoin();
			InterlockedIncrement(&_this->_ContentsSession);
			InterlockedDecrement(&_this->_AuthSession);
			limit--;
		}
		

		// 패킷 처리
		BEGIN("PART1");
		for (int i = 0; i < _this->_maxClient; i++)
		{
			session = sessionArray[i];
			if (session->_mode != CONTENTS)
				continue;
			// 로그아웃 처리 1단계
			if (session->_logOutFlag == true)
			{
				if (session->_sendFlag == TRUE)
					session->_mode = LOGOUT_IN_CONTENTS;
				else
				{
					session->_mode = WAIT_LOGOUT;
					InterlockedDecrement(&_this->_ContentsSession);
				}
			}
			else
			{
				int process = _this->CONTENTS_PACKET_PROCESS;
				StreamBuffer * packet = nullptr;
				while (process--)
				{
					//if (session->_completeRecvQueue.empty())
					if (session->_completeRecvQueue.DeQueue_UnSafe(&packet) == false)
						break;
					//packet = session->_completeRecvQueue.front();
					//session->_completeRecvQueue.pop();
					BEGIN(PACKET);
					session->OnContents_Packet(packet);
					END(PACKET);
					PacketFree(packet);
				}
			}
		}
		END("PART1");

		// update
		_this->OnContents_Update();

		//// 로그아웃 1단계
		//for (int i = 0; i < _this->_maxClient; i++)
		//{
		//	if (_this->_sessionArray[i]->_mode != CONTENTS)
		//		continue;

		//	if (_this->_sessionArray[i]->_logOutFlag != true)
		//		continue;

		//	_this->_sessionArray[i]->_mode = LOGOUT_IN_CONTENTS;
		//}


		// 릴리즈
		//BEGIN("PART3");
		//for (int i = 0; i < _this->_maxClient; i++)
		//{
		//	MMOSession * session = sessionArray[i];
		//	if (session->_mode != WAIT_LOGOUT)
		//		continue;
		//	
		//	closesocket(session->_socket);
		//	session->OnContents_ClientLeave();
		//	session->Clean();
		//	_this->_indexStack.Push(i);
		//	InterlockedDecrement(&_this->_TotalSession);
		//}
		//END("PART3");
		Sleep(_this->CONTENTS_SLEEP);
		END("THREAD_MAIN");
	}
	return 0;
}

unsigned int __stdcall MMOServer::ReleaseThreadMain(LPVOID param)
{
	MMOServer * _this = (MMOServer *)param;
	MMOSession ** sessionArray = _this->_sessionArray;
	MMOSession * session = nullptr;
	while (1)
	{
		for (int i = 0; i < _this->_maxClient; i++)
		{
			session = sessionArray[i];
			if (session->_mode == LOGOUT_IN_AUTH)
			{
				if (session->_sendFlag == TRUE)
					continue;

				session->_mode = WAIT_LOGOUT;
				session->OnAuth_ClientLeave(true);
				InterlockedDecrement(&_this->_AuthSession);
			}
		}
		// 로그아웃 2단계
		for (int i = 0; i < _this->_maxClient; i++)
		{
			session = sessionArray[i];
			if (session->_mode != LOGOUT_IN_CONTENTS)
				continue;

			if (session->_sendFlag == TRUE)
				continue;

			session->_mode = WAIT_LOGOUT;
			session->OnContents_ClientLeave();
			InterlockedDecrement(&_this->_ContentsSession);
		}

		for (int i = 0; i < _this->_maxClient; i++)
		{
			session = sessionArray[i];
			if (session->_mode != WAIT_LOGOUT)
				continue;

			closesocket(session->_socket);
			session->OnContents_ClientRelease();
			session->Clean();
			_this->_indexStack.Push(i);
			InterlockedDecrement(&_this->_TotalSession);
		}
		Sleep(_this->RELEASE_SLEEP);
	}
	return 0;
}

MMOServer::MMOServer()
{
}

MMOServer::~MMOServer()
{
}

void MMOServer::SetSessionArray(void * pointer, int index)
{
	_sessionArray[index] = (MMOSession *)pointer;
}

void MMOServer::Start(const char * fileName)
{
	LoadConfig(fileName);
	LOGGER.SetLogDir(&wstring(L"Server_LOG"));
	LOGGER.SetLogLevel(LOG_DEBUG);
	MMOSession::_packetCode = _packetCode;
	MMOSession::_XORKey1 = _XORKey1;
	MMOSession::_XORKey2 = _XORKey2;

	if (MAX_SESSION < _maxClient)
		_maxClient = MAX_SESSION;

	if (MAX_THREAD < _workerThreadCount)
		_workerThreadCount = MAX_THREAD;

	if (MAX_THREAD < _sendThreadCount)
		_sendThreadCount = MAX_THREAD;

	for (int i = _maxClient; i > 0; i--)
	{
		_indexStack.Push(i - 1);
	}

	_clientSeed = 0;

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, _activeWorkerThreadCount);
	for (int i = 0; i < _workerThreadCount; ++i)
		_workerThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	_acceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	_authThread = (HANDLE)_beginthreadex(NULL, 0, AuthThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	for (int i = 0; i < _sendThreadCount; ++i)
		_sendThread[i] = (HANDLE)_beginthreadex(NULL, 0, SendThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	_contentsThread = (HANDLE)_beginthreadex(NULL, 0, ContentsThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	_releaseThread = (HANDLE)_beginthreadex(NULL, 0, ReleaseThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);

	SetThreadPriority(_contentsThread, THREAD_PRIORITY_ABOVE_NORMAL);
}

void MMOServer::DisConnect(MMOSession * session)
{
	shutdown(session->_socket, SD_SEND);
}

void MMOServer::PrintInfo()
{
	_authFPS_Display = _authFPS;
	_contentsFPS_Display = _contentsFPS;

	InterlockedAdd(&_authFPS, -_authFPS);
	InterlockedAdd(&_contentsFPS, -_contentsFPS);


	printf_s("MMOServer\n");
	printf_s("------------------------------------------------------------------\n");
	printf_s("        Use Index Count  = %I64d\n", _maxClient - _indexStack.GetUseCount());
	printf_s("    Total Session Count  = %d\n", _TotalSession);
	printf_s("     Auth Session Count  = %d\n", _AuthSession);
	printf_s(" Contents Session Count  = %d\n", _ContentsSession);
	printf_s("     Packet Alloc Count  = %d\n", StreamBuffer::_allocCount);
	printf_s("\n");
	printf_s("          Accept Count = %d\n", _acceptCount);
	printf_s("      AcceptInfo Count = %d\n", _acceptQueue.GetUseCount());
	printf_s("\n");
	printf_s("       Auth FPS = %d\n", _authFPS_Display);
	printf_s("   Contents FPS = %d\n", _contentsFPS_Display);
	printf_s("\n");
	printf_s("     Accept TPS = %d\n", _acceptTPS);
	printf_s("       Recv TPS = %d\n", _recvTPS);
	printf_s("       Send TPS = %d\n", _sendTPS);
	printf_s("\n");
	printf_s("       NO index = %d\n", _noIndex);
	printf_s("------------------------------------------------------------------\n");

	InterlockedAdd(&_acceptTPS, -_acceptTPS);
	InterlockedAdd(&_recvTPS, -_recvTPS);
	InterlockedAdd(&_sendTPS, -_sendTPS);
}
