﻿//#include "pch.h"
#include "NetClient.h"
#include "CrashDump.h"

namespace MinLib
{
	extern CrashDump cd;

	/*----------------------------------------------------------*/
	// NetClient::NetClient (public)
	// 설명 : 생성자
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	NetClient::NetClient()
	{
		_workerThreadCount = 0;
		_clientSeed = 1;
		_serverPort = 0;

		LoadConfig("NetClient.cnf");

		//_acceptCount = 0;
		//_acceptTPS = 0;
		//_acceptTPSTime = GetTickCount64();
	}

	NetClient::NetClient(const char* configFile)
	{
		_workerThreadCount = 0;
		_clientSeed = 1;
		_serverPort = 0;

		LoadConfig(configFile);
	}

	/*----------------------------------------------------------*/
	// NetClient::~NetClient (public)
	// 설명 : 소멸자
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	NetClient::~NetClient()
	{
	}

	/*----------------------------------------------------------*/
	// NetClient::WorkerThreadMain (protected)
	// 설명 : Worker스레드 메인함수
	// 인자 : (LPVOID) 포인터
	// 리턴 : (unsigned int) 정상종료시 0
	/*----------------*////////////////////////*----------------*/
	unsigned int WINAPI NetClient::WorkerThreadMain(LPVOID lpParam)
	{
		NetClient* _this = (NetClient*)lpParam;

		while (1)
		{
			DWORD transBtyes = 0;
			LPOVERLAPPED overlapped = nullptr;
			Session* session = nullptr;
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
				shutdown(session->socket_, SD_SEND);
				_this->TryDisConnect(session);
			}
			else
			{
				// 수신완료
				if (&session->recvOverLapped_ == overlapped)
				{
					// 정상적 종료 상황
					if (transBtyes == 0)
					{
						//printf("정상종료 메시지 수신\n");
						shutdown(session->socket_, SD_SEND);
					}
					// 정상 수신완료
					else
					{
						session->recvQueue_.MoveWritePos(transBtyes);
						_this->RecvProc(session);
						_this->RecvPost(session);
					}
				}
				// 송신완료
				else if (&session->sendOverLapped_ == overlapped)
				{
					int size = session->sendCount_;
					for (int i = 0; i < size; i++)
					{
						PacketFree(session->sendArray_[i]);
					}
					session->sendCount_ = 0;
					session->sendFlag_ = FALSE;
					_this->OnSend(session->clientID_, transBtyes);
					//SendProc(session);
					_this->SendPost(session);
				}
				// 알수없는 상황
				else
					;
				// 여기서 io 카운트 감소
				int ioCount = InterlockedDecrement((LONG*)&session->ioCountUseFlag_.ioCount);
				// 연결종료 체크 구간
				if (ioCount == 0)
					_this->TryDisConnect(session);
			}
		}
		return 0;
	}

	/*----------------------------------------------------------*/
	// NetClient::ConnectThreadMain (protected)
	// 설명 : Connect스레드 메인함수
	// 인자 : (LPVOID) 포인터
	// 리턴 : (unsigned int) 정상종료시 0
	/*----------------*////////////////////////*----------------*/
	unsigned int WINAPI NetClient::ConnectThreadMain(LPVOID lpParam)
	{
		NetClient* _this = (NetClient*)lpParam;

		_this->_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		char reUseAddr = 1;
		setsockopt(_this->_listenSocket, SOL_SOCKET, SO_REUSEADDR, &reUseAddr, sizeof(reUseAddr));
		SOCKADDR_IN addr;
		ZeroMemory(&addr, sizeof(addr));
		//inet_pton(AF_INET, INADDR_ANY, &addr.sin_addr.s_addr);
		//addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//addr.sin_addr.s_addr = inet_addr(_this->_bindIP);
		inet_pton(AF_INET, _this->_bindIP, &addr.sin_addr);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(_this->_serverPort);
		//int retval = bind(_this->_listenSocket, (SOCKADDR*)&addr, sizeof(addr));
		//if (retval == SOCKET_ERROR)
		//{
		//	//LOG(Logger::LOG_LEVEL::LOG_LEVEL_ERROR, L"bind Error\n");
		//	int error = GetLastError();
		//	system("pause");
		//	return -1;
		//}
		int retval;
		while (1)
		{
			retval = connect(_this->_listenSocket, (SOCKADDR*)&addr, sizeof(addr));
			if (retval == SOCKET_ERROR)
			{
				DWORD error = GetLastError();
				//printf("연결 실패, 5초뒤 재시도\n");
				Sleep(5000);
			}
			else
				break;
		}
		_this->_sessionArray[0].Init(_this->_listenSocket, _this->_clientSeed);
		CreateIoCompletionPort((HANDLE)_this->_listenSocket, _this->_iocp, (ULONG_PTR)(&_this->_sessionArray[0]), 0);
		_this->OnConnect(_this->_clientSeed++);
		_this->RecvPost(_this->_sessionArray);
		int ioCount = InterlockedDecrement((LONG*)&_this->_sessionArray->ioCountUseFlag_.ioCount);
		if (ioCount == 0)
			_this->TryDisConnect(_this->_sessionArray);
		//listen(_this->_listenSocket, SOMAXCONN);
		//while (1)
		//{
		//	SOCKADDR_IN newAddr;
		//	int addrSize = sizeof(newAddr);
		//	SOCKET sock = accept(_this->_listenSocket, (SOCKADDR*)&newAddr, &addrSize);
		//	//_this->_acceptCount++;
		//	//InterlockedIncrement((LONG*)&_this->_acceptTPS);
		//	if (sock == INVALID_SOCKET)
		//	{
		//		int errorCode = GetLastError();
		//		if (errorCode == WSAEWOULDBLOCK)
		//		{
		//			Sleep(1000);
		//			continue;
		//		}
		//		// accept중 리슨소켓을 닫아서 이쪽으로 유도
		//		else if (errorCode == WSAEINTR)
		//			break;
		//		else if (errorCode == WSAENOTSOCK)
		//			break;
		//		// 뭔가 에러인부분
		//		continue;
		//	}
		//	// 가상 함수
		//	char buffer[128];
		//	inet_ntop(AF_INET, &newAddr.sin_addr, buffer, 128);
		//	if (!_this->OnConnectionRequest(buffer, ntohs(newAddr.sin_port)))
		//	{
		//		closesocket(sock);
		//		LOG(L"Client", LOG_WARNING, L"OnConnectionRequest False");
		//		continue;
		//	}

		//	int index;
		//	if (!_this->GetBlankIndex(&index))
		//	{
		//		closesocket(sock);
		//		LOG(L"Client", LOG_ERROR, L"NO INDEX");
		//		continue;
		//	}
		//	
		//	int64_t clientID = (_this->_clientSeed++ << 2 * 8) | index;
		//	_this->_sessionArray[index].Init(sock, clientID);
		//	Session * session = &_this->_sessionArray[index];
		//	int size = session->recvQueue_.GetUseSize();
		//
		//	CreateIoCompletionPort((HANDLE)sock, _this->_iocp, (ULONG_PTR)session, 0);
		//	// 가상 함수
		//	_this->OnClientJoin(session->clientID_, session);
		//	_this->RecvPost(session);
		//	int ioCount = InterlockedDecrement((LONG *)&session->ioCountUseFlag_.ioCount);
		//	if (ioCount == 0)
		//		_this->TryDisConnect(session);
		//}
		return 0;
	}

	bool NetClient::LoadConfig(const char* configFile)
	{
		if (!_configParser.OpenFile(configFile))
			throw;
		_configParser.GetValue_Str("BIND_IP", _bindIP, "SERVER");
		_configParser.GetValue_Int("BIND_PORT", _serverPort, "SERVER");
		_configParser.GetValue_Int("WORKER_THREAD", _workerThreadCount, "SERVER");
		//_configParser.GetValue_Int("CLIENT_MAX", _clientMAX, "SERVER");
		//_configParser.GetValue_Int("MONITOR_NO", _monitorNo, "SERVER");


		_configParser.GetValue_BYTE("PACKET_CODE", _packetCode, "SERVER");
		_configParser.GetValue_BYTE("PACKET_KEY1", (BYTE&)_XORKey1, "SERVER");
		_configParser.GetValue_BYTE("PACKET_KEY2", (BYTE&)_XORKey2, "SERVER");

		wstring logDirName(L"Server_LOG");
		LOGGER.SetLogDir(&logDirName);

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

	/*----------------------------------------------------------*/
	// NetClient::Start (public)
	// 설명 : 서버 시작하는 함수
	// 인자 : (int) 워커스레드 수<최대 MAXTHREADCOUNT> , (int) 포트번호, (bool) nagle여부 <미구현>
	// 리턴 : (bool) true
	/*----------------*////////////////////////*----------------*/
	bool NetClient::Start(bool nagle)
	{
		if (MAXTHREADCOUNT < _workerThreadCount)
			_workerThreadCount = MAXTHREADCOUNT;


		WSAData wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, _workerThreadCount);
		for (int i = 0; i < _workerThreadCount; ++i)
			_workerThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
		_acceptThread = (HANDLE)_beginthreadex(NULL, 0, ConnectThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);

		_status = SERVER_ON;

		return true;
	}

	/*----------------------------------------------------------*/
	// NetClient::Stop (public)
	// 설명 : 서버 종료 함수
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	void NetClient::Stop()
	{
		// accept스레드 종료대기
		closesocket(_listenSocket);
		WaitForSingleObject(_acceptThread, INFINITE);

		// 연결된 소켓 종료
		for (auto& session : _sessionArray)
		{
			if (session.socket_ != INVALID_SOCKET)
				closesocket(session.socket_);
		}

		// worker스레드 종료대기
		for (int i = 0; i < _workerThreadCount; ++i)
			PostQueuedCompletionStatus(_iocp, 0, 0, 0);
		WaitForMultipleObjects(_workerThreadCount, _workerThread, TRUE, INFINITE);

		_status = SERVER_OFF;

		WSACleanup();
	}

	/*----------------------------------------------------------*/
	// NetClient::GetClientCount (public)
	// 설명 : 현재 접속중인 클라이언트 수
	// 인자 : 
	// 리턴 : (int) 클라이언트 수
	/*----------------*////////////////////////*----------------*/
	int NetClient::GetClientCount()
	{
		return (int)(_clientMAX - _indexStack.GetUseCount());
	}

	/*----------------------------------------------------------*/
	// NetClient::TryDisConnect (protected)
	// 설명 : 내부에서 사용되는 세션 종료시도 , io카운트 감소시 호출된다
	// 인자 : (Session *) 세션포인터
	// 리턴 : 
	/*----------------*////////////////////////*----------------*/
	void NetClient::TryDisConnect(Session* session)
	{
		if (session == nullptr)
			return;

		if (InterlockedCompareExchange64((LONG64*)&session->ioCountUseFlag_, NULL, TRUE) == TRUE)
		{
			closesocket(session->socket_);
			OnClientLeave(session->clientID_);

			// boost queue
			StreamBuffer* packet = nullptr;
#ifdef BOOST
			while (session->sendQueue_.pop(packet))
			{
				PacketFree(packet);
			}
#else
			int sized = (int)session->sendQueue_.GetUseCount();
			while (sized--)
			{
				session->sendQueue_.DeQueue(&packet);
				PacketFree(packet);
			}
#endif

			int size = session->sendCount_;
			for (int i = 0; i < size; i++)
			{
				PacketFree(session->sendArray_[i]);
			}
			session->sendCount_ = 0;

			session->socket_ = INVALID_SOCKET;
			int sessionIndex = FindIndex(session->clientID_);
			session->clientID_ = -1;
			session->ioCountUseFlag_.useFlag = false;
			_indexStack.Push(sessionIndex);
		}
	}

	/*----------------------------------------------------------*/
	// NetClient::Disconnect (public)
	// 설명 : 외부에서 사용되는 클라이언트 접속 종료 요청
	// 인자 : (int64_t) 클라이언트ID
	// 리턴 : 성공여부
	/*----------------*////////////////////////*----------------*/
	bool NetClient::Disconnect(int64_t clientID)
	{
		int index = FindIndex(clientID);
		Session* session = &_sessionArray[index];
		if (session->clientID_ != clientID)
			return false;
		shutdown(session->socket_, SD_SEND);
		TryDisConnect(session);
		return true;
	}

	/*----------------------------------------------------------*/
	// NetClient::SendPacket (public)
	// 설명 : 외부에서 사용되는 패킷전송요청
	// 인자 : (int64_t) 클라이언트ID, (StreamBuffer *) 패킷포인터
	// 리턴 : (bool) 성공여부
	/*----------------*////////////////////////*----------------*/
	bool NetClient::SendPacket(int64_t clientID, StreamBuffer* packet)
	{
		//int index = FindIndex(clientID);
		Session* session = &_sessionArray[0];
		bool ret = false;
		int ioCount = InterlockedIncrement((LONG*)&session->ioCountUseFlag_.ioCount);
		if (ioCount != 1)
		{
			if (session->clientID_ == clientID)
			{
				if (session->ioCountUseFlag_.useFlag == TRUE)
				{
					if (session->socket_ != INVALID_SOCKET)
					{
						packet->AddRef();
						if (_InterlockedCompareExchange8((char*)&packet->headerEnterFlag_, true, false) == false)
							PutHeader(packet);
						while (!packet->encodeFlag_)
							Sleep(0);
#ifdef BOOST
						session->sendQueue_.push(packet);
#else
						session->sendQueue_.EnQueue(packet);
#endif
						ret = SendPost(session);
						//if (!ret)
						//	PacketFree(packet);

						//LOGHEX(L"packet", Logger::LOG_LEVEL::LOG_LEVEL_DEBUG, packet->GetBuffer(), packet->GetUseSize());
					}
				}
			}
		}
		ioCount = InterlockedDecrement((LONG*)&session->ioCountUseFlag_.ioCount);
		if (ioCount == 0)
			TryDisConnect(session);
		return ret;
	}

	void NetClient::Reconnect()
	{
		CloseHandle(_acceptThread);
		_acceptThread = (HANDLE)_beginthreadex(NULL, 0, ConnectThreadMain, this, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	}

	/*----------------------------------------------------------*/
	// NetClient::SendPost (protected)
	// 설명 : 실제로 packet을 send시도하는 함수
	// 인자 : (Session *) 세션 포인터
	// 리턴 : (bool) 전송 성공 여부
	/*----------------*////////////////////////*----------------*/
	bool NetClient::SendPost(Session* session)
	{
	SENDPOST:
		//if (session->sendOverLapped_.Internal == STATUS_PENDING)
		//	return false;
		// 한 쓰레드만 if안으로 가야한다.
		// send는 한번만 한다
		// send는 커널메모리 사용되기때문에, 페이지락도 걸림
		if (!InterlockedCompareExchange((LONG*)&session->sendFlag_, TRUE, FALSE))
		{
			//send큐 비어있을수 있다.
			//send완료통지받고 이후에 들어온다면..혹은 그거때문에 send가 이미 됬거나

#ifdef BOOST
			if (session->sendQueue_.empty())
			{
				session->sendFlag_ = FALSE;
				bool ret = true;
				if (!session->sendQueue_.empty())
					ret = SendPost(session);
				return ret;
			}
			WSABUF wsaBuf[sendArraySize];
			ZeroMemory(wsaBuf, sizeof(wsaBuf));
			int bufCount = 0;
			while (bufCount < sendArraySize)
			{
				StreamBuffer* packet = nullptr;
				if (!session->sendQueue_.pop(packet))
					break;
				char* buffer = packet->GetBuffer();
				int len = packet->GetUseSize();
				wsaBuf[bufCount].buf = buffer;
				wsaBuf[bufCount].len = len;
				session->sendArray_[bufCount] = packet;
				bufCount++;
			}
			session->sendCount_ = bufCount;
#else
			int useSize = (int)session->sendQueue_.GetUseCount();
			if (useSize == 0)
			{
				// 이순간 0임을 보장해야함
				session->sendFlag_ = FALSE;
				bool ret = true;
				if (session->sendQueue_.GetUseCount() != 0)
					goto SENDPOST;
				return ret;
			}

			WSABUF wsaBuf[sendArraySize];
			ZeroMemory(wsaBuf, sizeof(wsaBuf));
			int bufCount = min(useSize, sendArraySize);
			for (int i = 0; i < bufCount; i++)
			{
				StreamBuffer* packet = nullptr;
				session->sendQueue_.DeQueue(&packet);
				char* buffer = packet->GetBuffer();
				int len = packet->GetUseSize();
				wsaBuf[i].buf = buffer;
				wsaBuf[i].len = len;
				session->sendArray_[i] = packet;
			}
			session->sendCount_ = bufCount;
#endif

			ZeroMemory(&session->sendOverLapped_, sizeof(OVERLAPPED));

			DWORD transBytes = 0;
			DWORD flags = 0;
			InterlockedIncrement((LONG*)&session->ioCountUseFlag_.ioCount);
			int retval = WSASend(session->socket_, wsaBuf, bufCount, &transBytes, flags, &session->sendOverLapped_, NULL);
			if (retval == SOCKET_ERROR)
			{
				int errorCode = GetLastError();
				if (errorCode != ERROR_IO_PENDING)
				{
					if (errorCode == WSAENOBUFS)
						LOG(L"Client", LOG_ERROR, L"SessionID : %d , WSAENOBUFS", session->clientID_);
					else if (errorCode == WSAECONNRESET)
						;
					else if (errorCode == WSAESHUTDOWN)
						LOG(L"Client", LOG_WARNING, L"SessionID : %d , WSAESHUTDOWN", session->clientID_);
					else
					{
						printf("sendpost %d\n", errorCode);
						//session->sendFlag_ = FALSE;
						//return SendPost(session);
					}
					int ioCount = InterlockedDecrement((LONG*)&session->ioCountUseFlag_.ioCount);
					//shutdown(session->socket_, SD_SEND);
					if (ioCount == 0)
						TryDisConnect(session);
					return false;
				}
			}
			return true;
		}
		return true;
	}

	/*----------------------------------------------------------*/
	// NetClient::GetBlankIndex (protected)
	// 설명 : 스택에서 인덱스 번호 가져오기
	// 인자 : (int *) 인덱스가 저장될 변수
	// 리턴 : (bool) 스택 pop 성공여부
	/*----------------*////////////////////////*----------------*/
	bool NetClient::GetBlankIndex(int* index)
	{
		if (_indexStack.Pop(index))
			return true;
		return false;
	}

	/*----------------------------------------------------------*/
	// NetClient::FindIndex (protected)
	// 설명 : clientID를 세션 배열의 인덱스로 변환해주는 함수
	// 인자 : (int64_t) 클라이언트ID
	// 리턴 : (int) 변환된 인덱스
	/*----------------*////////////////////////*----------------*/
	int NetClient::FindIndex(int64_t clientID)
	{
		int64_t mask = 0xffff;
		return (int)(clientID & mask);
	}

	/*----------------------------------------------------------*/
	// NetClient::FindIndex (protected)
	// 설명 : 보낼 패킷에 헤더를 붙인다.
	// 인자 : 
	// 리턴 : 
	/*----------------*////////////////////////*----------------*/
	void NetClient::PutHeader(StreamBuffer* packet)
	{
		PACKET_HEADER header;
		int headerSize = sizeof(PACKET_HEADER);

		header.code = _packetCode;
		header.len = packet->GetUseSize() - headerSize;
		header.randXORKey = rand() % 256;
		header.checkSum = GetCheckSum(packet->GetBuffer() + headerSize, header.len);

		packet->FillHeader((char*)&header, sizeof(header));
		Encode(packet);
		packet->encodeFlag_ = true;
	}

	BYTE NetClient::GetCheckSum(char* buffer, int size)
	{
		BYTE checkSum = 0;
		while (size--)
		{
			checkSum += *(buffer++);
		}
		return checkSum;
	}

	void NetClient::XOR(char* buffer, int size, char key)
	{
		while (size--)
		{
			*(buffer++) ^= key;
		}
	}

	void NetClient::Encode(StreamBuffer* packet)
	{
		PACKET_HEADER* bufferPointer = (PACKET_HEADER*)packet->GetBuffer();

		XOR((char*)&bufferPointer->checkSum, bufferPointer->len + 1, bufferPointer->randXORKey);
		XOR((char*)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey1);
		XOR((char*)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey2);
	}

	bool NetClient::Decode(PACKET_HEADER* header, StreamBuffer* payloadPacket)
	{
		char key1 = _XORKey1;
		char key2 = _XORKey2;
		char* payloadPointer = payloadPacket->GetBuffer();
		int payloadSize = payloadPacket->GetUseSize();

		XOR(payloadPointer, payloadSize, key2);
		XOR((char*)&(header->randXORKey), 2, key2);

		XOR(payloadPointer, payloadSize, key1);
		XOR((char*)&(header->randXORKey), 2, key1);

		XOR(payloadPointer, payloadSize, header->randXORKey);
		XOR((char*)&header->checkSum, 1, header->randXORKey);

		BYTE checkSum = GetCheckSum(payloadPointer, payloadSize);
		if (checkSum != header->checkSum)
		{
			LOG(L"Client", LOG_ERROR, L"Decode CheckSum Error");
			return false;
		}
		return true;
	}

	/*----------------------------------------------------------*/
	// NetClient::RecvProc (protected)
	// 설명 : recv가 완료되었을때 호출될 함수
	// 인자 : (Session *) 세션 포인터
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	void NetClient::RecvProc(Session* session)
	{
		while (1)
		{
			PACKET_HEADER header;
			int headerSize = sizeof(header);
			int useSize = session->recvQueue_.GetUseSize();
			if (useSize < headerSize)
				break;
			int ret = session->recvQueue_.Peek((char*)&header, headerSize);

			if (header.code != _packetCode)
			{
				LOG(L"Client", LOG_ERROR, L"Packet Code is Wrong");
				shutdown(session->socket_, SD_SEND);
				break;
			}

			if (PacketSIZE < header.len)
			{
				LOG(L"Client", LOG_ERROR, L"Packet Len is Over 1500");
				shutdown(session->socket_, SD_SEND);
				break;
			}

			if (useSize < header.len + headerSize)
				break;

			session->recvQueue_.RemoveData(headerSize);
			// 패킷을 하나씩 꺼낸다
			StreamBuffer* packet = PacketAlloc(NetClient);
			ret = session->recvQueue_.Dequeue(packet->GetBuffer(), header.len);
			packet->MoveEndIndex(header.len);
			if (Decode(&header, packet))
				OnRecv(session->clientID_, packet);
			PacketFree(packet);
		}
	}

	/*----------------------------------------------------------*/
	// NetClient::RecvPost (protected)
	// 설명 : recv를 iocp에 등록
	// 인자 : (Session *) 세션 포인터
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	void NetClient::RecvPost(Session* session)
	{
		//리시브 큐가 꽉 찼는지 체크
		int fullSize = session->recvQueue_.GetBufferSize();
		int freeSize = session->recvQueue_.GetFreeSize();
		// 리시브큐 5% 미만 남음
		if (fullSize / 100 * 95 > freeSize)
		{
			// 종료
			LOG(L"Client", LOG_ERROR, L"SessionID : %d , RecvQueue Full", session->clientID_);
			shutdown(session->socket_, SD_SEND);
			return;
		}
		// wsabuf 세팅
		WSABUF wsaBuf[2];
		wsaBuf[0].buf = session->recvQueue_.GetWriteBufferPtr();
		wsaBuf[0].len = session->recvQueue_.GetNotBrokenPutSize();
		wsaBuf[1].buf = session->recvQueue_.GetBufferPtr();
		wsaBuf[1].len = freeSize - wsaBuf[0].len;
		//overlapped 초기화
		ZeroMemory(&session->recvOverLapped_, sizeof(OVERLAPPED));
		//iocount++
		InterlockedIncrement((LONG*)&session->ioCountUseFlag_.ioCount);
		//wsaRecv()
		DWORD transBytes = 0;
		DWORD flags = 0;
		int check = 0;
		(freeSize == wsaBuf[0].len) ? (check = 1) : (check = 2);
		int retval = WSARecv(session->socket_, wsaBuf, 2, &transBytes, &flags, &session->recvOverLapped_, NULL);
		//에러처리
		if (retval == SOCKET_ERROR)
		{
			int errorCode = GetLastError();
			if (errorCode != ERROR_IO_PENDING)
			{
				if (errorCode == WSAENOBUFS)
					LOG(L"Client", LOG_ERROR, L"SessionID : %d , WSAENOBUFS", session->clientID_);
				else if (errorCode == WSAECONNRESET)
					;
				else
				{
					printf("recvpost %d socket : %d\n", errorCode, (int)session->socket_);
					if (session->socket_ == INVALID_SOCKET)
						cd.Crash();
				}
				// 수신 등록 실패, 종료코드
				// LOG
				int ioCount = InterlockedDecrement((LONG*)&session->ioCountUseFlag_.ioCount);
				//shutdown(session->socket_, SD_SEND);
				if (ioCount == 0)
					TryDisConnect(session);
			}
		}
	}
}