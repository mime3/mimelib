#pragma once
#include "stdafx.h"

void MakePacket_Move_Start(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_CS_MOVE_START;
	ph.bySize = 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

void MakePacket_Move_Stop(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_CS_MOVE_STOP;
	ph.bySize = 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

void MakePacket_Attack1(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_CS_ATTACK1;
	ph.bySize = 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

void MakePacket_Attack2(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_CS_ATTACK2;
	ph.bySize = 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

void MakePacket_Attack3(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos)
{
	PACKET_HEADER ph;
	ph.byCode = dfPACKET_CODE;
	ph.byType = dfPACKET_CS_ATTACK3;
	ph.bySize = 1 + 2 + 2;

	*packet = new MinLib::StreamBuffer(sizeof(ph) + ph.bySize + sizeof(dfNETWORK_PACKET_END));
	(*packet)->BlankHeader(sizeof(ph));

	**packet << direction << pos << dfNETWORK_PACKET_END;

	(*packet)->FillHeader((char *)&ph, sizeof(ph));
}

void SendPacket(MinLib::StreamBuffer * packet, Session * session)
{
	session->sendStream.Enqueue(packet->GetBuffer(), packet->GetSize());
	delete packet;
}