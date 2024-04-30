#pragma once

void RecvPacket_CS_Move_Start(Session * session, MinLib::StreamBuffer * payload);

void RecvPacket_CS_Move_Stop(Session * session, MinLib::StreamBuffer * payload);

void RecvPacket_CS_Attack1(Session * session, MinLib::StreamBuffer * payload);

void RecvPacket_CS_Attack2(Session * session, MinLib::StreamBuffer * payload);

void RecvPacket_CS_Attack3(Session * session, MinLib::StreamBuffer * payload);

void RecvPacket_CS_Echo(Session * session, MinLib::StreamBuffer * payload);