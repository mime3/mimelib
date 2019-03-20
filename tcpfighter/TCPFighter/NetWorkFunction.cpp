#include "stdafx.h"

extern Session mySession;
extern WCHAR g_IP[];
extern Player * g_player;
extern CList<Object *>	g_list;

/*----------------------------------------------------------*/
// ::WsaInit
// 설명 : wsa세팅, 소켓세팅, connect
// 인자 : (HWND) 윈도우 핸들
// 리턴 :
/*----------------*////////////////////////*----------------*/
void WsaInit(HWND hWnd)
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	mySession.sock = socket(AF_INET, SOCK_STREAM, 0);
	ZeroMemory(&mySession.addr, sizeof(mySession.addr));
	char ansiIP[32];
	WideCharToMultiByte(CP_ACP, 0, g_IP, -1, ansiIP, 128, NULL, NULL);
	inet_pton(AF_INET, ansiIP, &mySession.addr.sin_addr.s_addr);
	mySession.addr.sin_port = htons(PORT_NUMBER);
	mySession.addr.sin_family = AF_INET;
#ifdef TCPNODELAY
	char tcpNoDelay = 1;
	setsockopt(mySession.sock, IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(tcpNoDelay));
#endif // TCPNODELAY
	WSAAsyncSelect(mySession.sock, hWnd, UM_NETWORK, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	connect(mySession.sock, (SOCKADDR*)&mySession.addr, sizeof(mySession.addr));
}

/*----------------------------------------------------------*/
// ::PacketProc
// 설명 : 링버퍼에서 패킷을 1개 꺼낸다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void PacketProc()
{
	while (1)
	{
		int payloadSize;
		int retval;
		PacketHeader packetHeader;
		// 헤더부터 읽어야 될때
		if (!mySession.headerSaveFlag)
		{
			retval = mySession.resvStream->Peek((char*)&packetHeader, sizeof(packetHeader));
			if (retval < sizeof(packetHeader))
				break;
			mySession.resvStream->RemoveData(retval);
			if (packetHeader.code != 0x89)
				break;
			payloadSize = packetHeader.size + 1;
		}
		// payload만 읽으면 될때
		else
			payloadSize = mySession.saveHeader.size + 1;
		
		StreamBuffer packet(payloadSize);
		retval = mySession.resvStream->Peek(packet.GetBuffer(), payloadSize);
		if (retval < payloadSize)
		{
			if (!mySession.headerSaveFlag)
			{
				mySession.headerSaveFlag = true;
				mySession.saveHeader = packetHeader;
			}
			break;
		}
		// EndCode 빼고 지움
		mySession.resvStream->RemoveData(retval - 1);
		packet.moveEndIndex();

		BYTE EndCode;
		retval = mySession.resvStream->Dequeue((char*)&EndCode, sizeof(BYTE));
		if (EndCode != 0x79)
			printf_s("EndCode is not Correct!\n");
		//END코드 제거된 패킷
		else
		{
			if (mySession.headerSaveFlag)
			{
				mySession.headerSaveFlag = false;
				PacketParse(mySession.saveHeader.type, packet);
			}
			else
				PacketParse(packetHeader.type, packet);
		}
	}
}

/*----------------------------------------------------------*/
// ::ProcConnect
// 설명 : 서버로부터 연결을 수락받았을때 처리부
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ProcConnect()
{
	mySession.connected = true;
	mySession.sendFlag = true;
}

/*----------------------------------------------------------*/
// ::ProcRead
// 설명 : 서버로부터 데이터를 전송받았을때 처리부
//  ㄴ  : 소켓으로부터 recv해서 링버퍼에 넣는다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ProcRead()
{

	char * writePointer = mySession.resvStream->GetWriteBufferPtr();
	int putSize = mySession.resvStream->GetNotBrokenPutSize();

	int retval = recv(mySession.sock, writePointer, putSize, 0);
	mySession.resvStream->MoveWritePos(retval);
	// RST처리
	if (retval == 0 || retval == SOCKET_ERROR)
	{
		// 받을게 원래 없다
		if (GetLastError() == WSAEWOULDBLOCK)
			return;
		ProcClose();
		return;
	}
	mySession.totalRecv += retval;
	PacketProc();
}

/*----------------------------------------------------------*/
// ::ProcWrite
// 설명 : 송신이 불가능에서 가능으로 바뀌었을때 처리부
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ProcWrite()
{
	mySession.sendFlag = true;
	SendPacket(nullptr, 0);
}

/*----------------------------------------------------------*/
// ::ProcClose
// 설명 : 연결이 종료됬을때 처리부
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ProcClose()
{
	closesocket(mySession.sock);
	PostQuitMessage(0);
}

/*----------------------------------------------------------*/
// ::SendPacket
// 설명 : 패킷을 send링버퍼에 넣고 send링버퍼를 모두 send한다.
// 인자 : (char *)패킷, (int) 패킷크기
// 리턴 :
/*----------------*////////////////////////*----------------*/
void SendPacket(char * buffer, int size)
{
	do
	{
		if (mySession.connected == false)
			return;
		int retval = mySession.sendStream->Enqueue(buffer, size);
		// send 링버퍼 포화
		if (retval != size)
		{
			ProcClose();
			return;
		}
		if (mySession.sendFlag == false)
			return;
		while (1)
		{
			int sendSize = mySession.sendStream->GetNotBrokenGetSize();
			retval = send(mySession.sock, mySession.sendStream->GetReadBufferPtr(), sendSize, 0);
			// send 링버퍼가 비워짐
			if (retval == 0)
				return;
			// send불가능상태
			else if (retval == SOCKET_ERROR)
			{
				if (GetLastError() == WSAEWOULDBLOCK)
				{
					mySession.sendFlag = false;
					return;
				}
				int error = WSAGetLastError();
				ProcClose();
				return;
			}
			mySession.sendStream->RemoveData(retval);
		}
		break;
	} while (!(buffer == nullptr || size == 0));
}

/*----------------------------------------------------------*/
// ::PacketParse
// 설명 : 패킷의 Payload를 타입으로 구분하여 데이터를 빼낸다.
//  ㄴ  : 빼낸 내용으로 처리할것도 처리한다.
// 인자 : (BYTE) 패킷타입, (StreamBuffer&) Payload
// 리턴 :
/*----------------*////////////////////////*----------------*/
void PacketParse(BYTE packetType, StreamBuffer &payload)
{
	DWORD	ID;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
	BYTE	HP;
	switch (packetType)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		payload >> ID >> Direction >> X >> Y >> HP;
		Player * player = new Player(ID, Direction, Pos{ X,Y }, HP);
		player->SetOwner();
		g_player = player;
		printf_s("Received Packet : dfPACKET_SC_CREATE_MY_CHARACTER\n ID:%d, Direction:%d, X:%d, Y:%d, HP:%d\n",ID, Direction, X, Y, HP);
		break;
	}
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		payload >> ID >> Direction >> X >> Y >> HP;
		Player * player = new Player(ID, Direction, Pos{ X,Y }, HP);
		printf_s("Received Packet : dfPACKET_SC_CREATE_OTHER_CHARACTER\nID:%d, Direction:%d, X:%d, Y:%d, HP:%d\n", ID, Direction, X, Y, HP);
		break;
	}
	case dfPACKET_SC_DELETE_CHARACTER:
	{
		payload >> ID;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		TargetPlayer->Die();
		printf_s("Received Packet : dfPACKET_SC_DELETE_CHARACTER\nID:%d, \n", ID);
		break;
	}
	case dfPACKET_SC_MOVE_START:
	{
		payload >> ID >> Direction >> X >> Y;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		if (Direction < ACTION_MOVE_LL || ACTION_MOVE_LD < Direction)
			break;
		TargetPlayer->ForceSetPos(Pos{ X,Y });
		TargetPlayer->Command((TAG_ACTION)Direction);
		printf_s("Received Packet : dfPACKET_SC_MOVE_START\nID:%d, Direction:%d, X:%d, Y:%d\n", ID, Direction, X, Y);
		break;
	}
	case dfPACKET_SC_MOVE_STOP:
	{
		payload >> ID >> Direction >> X >> Y;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		TargetPlayer->ForceSetPos(Pos{ X,Y });
		TargetPlayer->ChangeDirection((TAG_ACTION)Direction);
		TargetPlayer->Command(STAND);
		printf_s("Received Packet : dfPACKET_SC_MOVE_STOP\nID:%d, Direction:%d, X:%d, Y:%d\n", ID, Direction, X, Y);
		break;
	}
	case dfPACKET_SC_ATTACK1:
	{
		payload >> ID >> Direction >> X >> Y;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		TargetPlayer->ForceSetPos(Pos{ X,Y });
		TargetPlayer->ChangeDirection((TAG_ACTION)Direction);
		TargetPlayer->Command(ATTACK_TYPE_1);
		printf_s("Received Packet : dfPACKET_SC_ATTACK1\nID:%d, Direction:%d, X:%d, Y:%d\n", ID, Direction, X, Y);
		break;
	}
	case dfPACKET_SC_ATTACK2:
	{
		payload >> ID >> Direction >> X >> Y;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		TargetPlayer->ForceSetPos(Pos{ X,Y });
		TargetPlayer->ChangeDirection((TAG_ACTION)Direction);
		TargetPlayer->Command(ATTACK_TYPE_2);
		printf_s("Received Packet : dfPACKET_SC_ATTACK2\nID:%d, Direction:%d, X:%d, Y:%d\n", ID, Direction, X, Y);
		break;
	}
	case dfPACKET_SC_ATTACK3:
	{
		payload >> ID >> Direction >> X >> Y;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		TargetPlayer->ForceSetPos(Pos{ X,Y });
		TargetPlayer->ChangeDirection((TAG_ACTION)Direction);
		TargetPlayer->Command(ATTACK_TYPE_3);
		printf_s("Received Packet : dfPACKET_SC_ATTACK3\nID:%d, Direction:%d, X:%d, Y:%d\n", ID, Direction, X, Y);
		break;
	}
	case dfPACKET_SC_DAMAGE:
	{
		DWORD	AttackID;
		DWORD	DamageID;
		BYTE	DamageHP;
		payload >> AttackID >> DamageID >> DamageHP;
		Player * TargetPlayer = FindUser(DamageID);
		if (TargetPlayer == nullptr)
			break;
		Effect * damageEffect = new Effect(Pos{ 0,0 }, EFFECT_SPARK_01, 0);
		damageEffect->ChangeColor(Object::RED);
		damageEffect->ChangePosAndDelay(AttackID, DamageID);
		TargetPlayer->ChangeHP(DamageHP);
		printf_s("Received Packet : dfPACKET_SC_DAMAGE\nATTACK_ID:%d, DAMAGE_ID:%d, DAMAGE_HP:%d\n", AttackID, DamageID, DamageHP);
		break;
	}
	case dfPACKET_SC_SYNC:
	{
		DWORD ID;
		WORD x;
		WORD y;
		payload >> ID >> x >> y;
		Player * TargetPlayer = FindUser(ID);
		if (TargetPlayer == nullptr)
			break;
		TargetPlayer->ForceSetPos(Pos{ x, y });
		printf_s("Received Packet : dfPACKET_SC_SYNC\nID:%d, X:%d, Y:%d\n", ID, x, y);
		break;
	}
	default:
		break;
	}
}

/*----------------------------------------------------------*/
// ::MakeSendPacket
// 설명 : 인자로 받은 타입으로 패킷을 만드는 함수를 호출한다.
// 인자 : (TAG_ACTION)타입
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakeSendPacket(TAG_ACTION action)
{
	switch (action)
	{
	case ACTION_MOVE_LL:
	case ACTION_MOVE_LU:
	case ACTION_MOVE_UU:
	case ACTION_MOVE_RU:
	case ACTION_MOVE_RR:
	case ACTION_MOVE_RD:
	case ACTION_MOVE_DD:
	case ACTION_MOVE_LD:
		MakeSendMovePacket((BYTE)action , (WORD)g_player->GetPos().x, (WORD)g_player->GetPos().y);
		break;
	case ATTACK_TYPE_1: 
		MakeSendAttackPacket(dfPACKET_CS_ATTACK1, g_player->GetDirection(), (WORD)g_player->GetPos().x, (WORD)g_player->GetPos().y);
		break;
	case ATTACK_TYPE_2:
		MakeSendAttackPacket(dfPACKET_CS_ATTACK2, g_player->GetDirection(), (WORD)g_player->GetPos().x, (WORD)g_player->GetPos().y);
		break;
	case ATTACK_TYPE_3:
		MakeSendAttackPacket(dfPACKET_CS_ATTACK3, g_player->GetDirection(), (WORD)g_player->GetPos().x, (WORD)g_player->GetPos().y);
		break;
	case STAND:
		MakeSendStandPacket(g_player->GetDirection(), (WORD)g_player->GetPos().x, (WORD)g_player->GetPos().y);
		break;
	}
}

/*----------------------------------------------------------*/
// ::MakeSendMovePacket
// 설명 : 이동패킷을 만들고 전송한다.
// 인자 : (BYTE)방향, (WORD)x좌표, (WORD)y좌표
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakeSendMovePacket(BYTE direction, WORD x, WORD y)
{
	PacketHeader header{ 0x89, 5, dfPACKET_CS_MOVE_START, 0 };
	BYTE endCode = (BYTE)0x79;
	StreamBuffer stream(sizeof(header) + 5 + sizeof(endCode));
	stream << header << direction << x << y << endCode;
	SendPacket((char*)stream.GetBuffer(), stream.GetSize());
}

/*----------------------------------------------------------*/
// ::MakeSendAttackPacket
// 설명 : 공격패킷을 만들고 전송한다.
// 인자 : (int)공격 타입, (BYTE)방향, (WORD)x좌표, (WORD)y좌표
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakeSendAttackPacket(int type, BYTE direction, WORD x, WORD y)
{
	PacketHeader header{ 0x89, 5, (BYTE)type, 0 };
	BYTE endCode = (BYTE)0x79;
	StreamBuffer stream(sizeof(header) + 5 + sizeof(endCode));
	stream << header << direction << x << y << endCode;
	SendPacket((char*)stream.GetBuffer(), stream.GetSize());
}

/*----------------------------------------------------------*/
// ::MakeSendStandPacket
// 설명 : 정지패킷을 만들고 전송한다.
// 인자 : (BYTE)방향, (WORD)x좌표, (WORD)y좌표
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakeSendStandPacket(BYTE direction, WORD x, WORD y)
{
	PacketHeader header{ 0x89, 5, dfPACKET_CS_MOVE_STOP, 0 };
	BYTE endCode = (BYTE)0x79;
	StreamBuffer stream(sizeof(header) + 5 + sizeof(endCode));
	stream << header << direction << x << y << endCode;
	SendPacket((char*)stream.GetBuffer(), stream.GetSize());
}
