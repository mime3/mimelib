#pragma once

void RecvPacket_CS_Move_Start(Session * session, StreamBuffer * payload);

void RecvPacket_CS_Move_Stop(Session * session, StreamBuffer * payload);

void RecvPacket_CS_Attack1(Session * session, StreamBuffer * payload);

void RecvPacket_CS_Attack2(Session * session, StreamBuffer * payload);

void RecvPacket_CS_Attack3(Session * session, StreamBuffer * payload);

void RecvPacket_CS_Echo(Session * session, StreamBuffer * payload);