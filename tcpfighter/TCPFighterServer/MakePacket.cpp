#include "stdafx.h"
extern map<SOCKET, Session *> g_user_socket;

/*----------------------------------------------------------*/
// ::MakePacket_Create_My_Character
// 설명 : 클라이언트 자신의 캐릭터 생성 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치, (BYTE) HP
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Create_My_Character(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos, BYTE HP)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	ph.bySize = 4 + 1 + 2 + 2 + 1;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << HP << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Create_Other_Character
// 설명 : 다른 클라이언트의 캐릭터 생성 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치, (BYTE) HP
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Create_Other_Character(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos, BYTE HP)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
	ph.bySize = 4 + 1 + 2 + 2 + 1;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << HP << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Delete_Character
// 설명 : 다른 클라이언트 캐릭터 삭제 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Delete_Character(MinLib::StreamBuffer ** packet, int ID)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_DELETE_CHARACTER;
	ph.bySize = 4;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Move_Start
// 설명 : 캐릭터의 이동 시작 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Move_Start(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_MOVE_START;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Move_Stop
// 설명 : 캐릭터의 정지 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Move_Stop(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_MOVE_STOP;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Attack1
// 설명 : 캐릭터의 공격 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Attack1(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ATTACK1;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Attack2
// 설명 : 캐릭터의 공격 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Attack2(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ATTACK2;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Attack3
// 설명 : 캐릭터의 공격 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (BYTE) 방향, (Pos) 위치
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Attack3(MinLib::StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ATTACK3;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Damage
// 설명 : 데미지 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 공격자 고유번호, (int) 피격자 고유번호, (BYTE) 피격후HP
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Damage(MinLib::StreamBuffer ** packet, int AttackID, int DamageID, BYTE damageHP)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_DAMAGE;
	ph.bySize = 4 + 4 + 1;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << AttackID << DamageID << damageHP << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Sync
// 설명 : 데드레커닝 후 위치를 보정할 싱크 패킷 만들기
// 인자 : (MinLib::StreamBuffer **) 직렬화버퍼 2중포인터, (int) 고유번호, (Pos) 위치
// 리턴 :
/*----------------*////////////////////////*----------------*/
void MakePacket_Sync(MinLib::StreamBuffer ** packet, int ID, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_SYNC;
	ph.bySize = 4 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

// ::MakePacket_Echo
void MakePacket_Echo(MinLib::StreamBuffer ** packet, DWORD time)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ECHO;
	ph.bySize = 4;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << time << dfNETWORK_PACKET_END;
	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::UniCast
// 설명 : 세션에게 패킷 보내기
// 인자 : (MinLib::StreamBuffer *) 직렬화버퍼 포인터, (Session *) 세션 포인터, (bool) delete여부
// 리턴 :
/*----------------*////////////////////////*----------------*/
void UniCast(MinLib::StreamBuffer * stream, Session * session, bool deletePacket)
{
	int size = stream->GetSize();
	int retval = session->sendStream.Enqueue(stream->GetBuffer(), size);
	if (size != retval)
	{
		LogoutProcess(session->socket);
		LOG(Logger::LOG_LEVEL_ERROR, L"SendRingBuffer Full Socket %d\n", session->socket);
	}
	if (deletePacket)
		delete stream;
}

/*----------------------------------------------------------*/
// ::BroadCast_All
// 설명 : 전체에게 패킷 보내기
// 인자 : (MinLib::StreamBuffer *) 직렬화버퍼 포인터, (Session *) 세션 포인터, (bool) 세션에게 전송 여부
// 리턴 :
/*----------------*////////////////////////*----------------*/
void BroadCast_All(MinLib::StreamBuffer * stream, Session * session, bool dontSend)
{
	map<SOCKET, Session *>::iterator iter = g_user_socket.begin();
	map<SOCKET, Session *>::iterator iterEnd = g_user_socket.end();
	for (; iter != iterEnd; ++iter)
	{
		if (iter->second == session && dontSend)
			continue;
		UniCast(stream, iter->second, false);
	}
	delete stream;
}

/*----------------------------------------------------------*/
// ::BroadCast_Sector
// 설명 : 주변 섹터에게 패킷 보내기
// 인자 : (MinLib::StreamBuffer *) 직렬화버퍼 포인터, (Session *) 세션 포인터, (bool) 세션에게 전송 여부
// 리턴 :
/*----------------*////////////////////////*----------------*/
void BroadCast_Sector(MinLib::StreamBuffer * stream, Session * session, bool dontSend)
{
	SECTOR_POS pos = SECTOR_MANAGER.CalculateSector(session->character);
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, pos);
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x - 1, pos.y - 1 });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x - 1, pos.y });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x - 1, pos.y + 1 });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x, pos.y - 1 });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x, pos.y + 1 });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x + 1, pos.y - 1 });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x + 1, pos.y });
	SECTOR_MANAGER.BroadCast_Sector(stream, session, dontSend, SECTOR_POS{ pos.x + 1, pos.y + 1 });
	delete stream;
}
