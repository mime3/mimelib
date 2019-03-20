#pragma once

void MakePacket_Move_Start(StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Move_Stop(StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Attack1(StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Attack2(StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Attack3(StreamBuffer ** packet, BYTE direction, Pos pos);
void SendPacket(StreamBuffer * packet, Session * session);