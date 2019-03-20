#include "stdafx.h"

extern Session mySession;
extern WCHAR g_IP[];
extern Player * g_player;
extern CList<Object *>	g_list;

/*----------------------------------------------------------*/
// ::WsaInit
// ���� : wsa����, ���ϼ���, connect
// ���� : (HWND) ������ �ڵ�
// ���� :
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
// ���� : �����ۿ��� ��Ŷ�� 1�� ������.
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void PacketProc()
{
	while (1)
	{
		int payloadSize;
		int retval;
		PacketHeader packetHeader;
		// ������� �о�� �ɶ�
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
		// payload�� ������ �ɶ�
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
		// EndCode ���� ����
		mySession.resvStream->RemoveData(retval - 1);
		packet.moveEndIndex();

		BYTE EndCode;
		retval = mySession.resvStream->Dequeue((char*)&EndCode, sizeof(BYTE));
		if (EndCode != 0x79)
			printf_s("EndCode is not Correct!\n");
		//END�ڵ� ���ŵ� ��Ŷ
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
// ���� : �����κ��� ������ �����޾����� ó����
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void ProcConnect()
{
	mySession.connected = true;
	mySession.sendFlag = true;
}

/*----------------------------------------------------------*/
// ::ProcRead
// ���� : �����κ��� �����͸� ���۹޾����� ó����
//  ��  : �������κ��� recv�ؼ� �����ۿ� �ִ´�.
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void ProcRead()
{

	char * writePointer = mySession.resvStream->GetWriteBufferPtr();
	int putSize = mySession.resvStream->GetNotBrokenPutSize();

	int retval = recv(mySession.sock, writePointer, putSize, 0);
	mySession.resvStream->MoveWritePos(retval);
	// RSTó��
	if (retval == 0 || retval == SOCKET_ERROR)
	{
		// ������ ���� ����
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
// ���� : �۽��� �Ұ��ɿ��� �������� �ٲ������ ó����
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void ProcWrite()
{
	mySession.sendFlag = true;
	SendPacket(nullptr, 0);
}

/*----------------------------------------------------------*/
// ::ProcClose
// ���� : ������ ��������� ó����
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void ProcClose()
{
	closesocket(mySession.sock);
	PostQuitMessage(0);
}

/*----------------------------------------------------------*/
// ::SendPacket
// ���� : ��Ŷ�� send�����ۿ� �ְ� send�����۸� ��� send�Ѵ�.
// ���� : (char *)��Ŷ, (int) ��Ŷũ��
// ���� :
/*----------------*////////////////////////*----------------*/
void SendPacket(char * buffer, int size)
{
	do
	{
		if (mySession.connected == false)
			return;
		int retval = mySession.sendStream->Enqueue(buffer, size);
		// send ������ ��ȭ
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
			// send �����۰� �����
			if (retval == 0)
				return;
			// send�Ұ��ɻ���
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
// ���� : ��Ŷ�� Payload�� Ÿ������ �����Ͽ� �����͸� ������.
//  ��  : ���� �������� ó���Ұ͵� ó���Ѵ�.
// ���� : (BYTE) ��ŶŸ��, (StreamBuffer&) Payload
// ���� :
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
// ���� : ���ڷ� ���� Ÿ������ ��Ŷ�� ����� �Լ��� ȣ���Ѵ�.
// ���� : (TAG_ACTION)Ÿ��
// ���� :
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
// ���� : �̵���Ŷ�� ����� �����Ѵ�.
// ���� : (BYTE)����, (WORD)x��ǥ, (WORD)y��ǥ
// ���� :
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
// ���� : ������Ŷ�� ����� �����Ѵ�.
// ���� : (int)���� Ÿ��, (BYTE)����, (WORD)x��ǥ, (WORD)y��ǥ
// ���� :
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
// ���� : ������Ŷ�� ����� �����Ѵ�.
// ���� : (BYTE)����, (WORD)x��ǥ, (WORD)y��ǥ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakeSendStandPacket(BYTE direction, WORD x, WORD y)
{
	PacketHeader header{ 0x89, 5, dfPACKET_CS_MOVE_STOP, 0 };
	BYTE endCode = (BYTE)0x79;
	StreamBuffer stream(sizeof(header) + 5 + sizeof(endCode));
	stream << header << direction << x << y << endCode;
	SendPacket((char*)stream.GetBuffer(), stream.GetSize());
}
