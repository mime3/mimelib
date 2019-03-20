#include "stdafx.h"
extern map<SOCKET, Session *> g_user_socket;

/*----------------------------------------------------------*/
// ::MakePacket_Create_My_Character
// ���� : Ŭ���̾�Ʈ �ڽ��� ĳ���� ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ, (BYTE) HP
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Create_My_Character(StreamBuffer ** packet, int ID, BYTE direction, Pos pos, BYTE HP)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	ph.bySize = 4 + 1 + 2 + 2 + 1;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << HP << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Create_Other_Character
// ���� : �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ, (BYTE) HP
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Create_Other_Character(StreamBuffer ** packet, int ID, BYTE direction, Pos pos, BYTE HP)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
	ph.bySize = 4 + 1 + 2 + 2 + 1;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << HP << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Delete_Character
// ���� : �ٸ� Ŭ���̾�Ʈ ĳ���� ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Delete_Character(StreamBuffer ** packet, int ID)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_DELETE_CHARACTER;
	ph.bySize = 4;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Move_Start
// ���� : ĳ������ �̵� ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Move_Start(StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_MOVE_START;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Move_Stop
// ���� : ĳ������ ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Move_Stop(StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_MOVE_STOP;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Attack1
// ���� : ĳ������ ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Attack1(StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ATTACK1;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Attack2
// ���� : ĳ������ ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Attack2(StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ATTACK2;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Attack3
// ���� : ĳ������ ���� ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (BYTE) ����, (Pos) ��ġ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Attack3(StreamBuffer ** packet, int ID, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ATTACK3;
	ph.bySize = 4 + 1 + 2 + 2;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Damage
// ���� : ������ ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ ������ȣ, (int) �ǰ��� ������ȣ, (BYTE) �ǰ���HP
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Damage(StreamBuffer ** packet, int AttackID, int DamageID, BYTE damageHP)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_DAMAGE;
	ph.bySize = 4 + 4 + 1;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << AttackID << DamageID << damageHP << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::MakePacket_Sync
// ���� : ���巹Ŀ�� �� ��ġ�� ������ ��ũ ��Ŷ �����
// ���� : (StreamBuffer **) ����ȭ���� 2��������, (int) ������ȣ, (Pos) ��ġ
// ���� :
/*----------------*////////////////////////*----------------*/
void MakePacket_Sync(StreamBuffer ** packet, int ID, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_SYNC;
	ph.bySize = 4 + 2 + 2;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << ID << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

// ::MakePacket_Echo
void MakePacket_Echo(StreamBuffer ** packet, DWORD time)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_SC_ECHO;
	ph.bySize = 4;

	*packet = new StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << time << dfNETWORK_PACKET_END;
	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

/*----------------------------------------------------------*/
// ::UniCast
// ���� : ���ǿ��� ��Ŷ ������
// ���� : (StreamBuffer *) ����ȭ���� ������, (Session *) ���� ������, (bool) delete����
// ���� :
/*----------------*////////////////////////*----------------*/
void UniCast(StreamBuffer * stream, Session * session, bool deletePacket)
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
// ���� : ��ü���� ��Ŷ ������
// ���� : (StreamBuffer *) ����ȭ���� ������, (Session *) ���� ������, (bool) ���ǿ��� ���� ����
// ���� :
/*----------------*////////////////////////*----------------*/
void BroadCast_All(StreamBuffer * stream, Session * session, bool dontSend)
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
// ���� : �ֺ� ���Ϳ��� ��Ŷ ������
// ���� : (StreamBuffer *) ����ȭ���� ������, (Session *) ���� ������, (bool) ���ǿ��� ���� ����
// ���� :
/*----------------*////////////////////////*----------------*/
void BroadCast_Sector(StreamBuffer * stream, Session * session, bool dontSend)
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
