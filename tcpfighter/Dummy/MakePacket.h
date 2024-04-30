#pragma once

void MakePacket_Move_Start(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Move_Stop(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Attack1(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Attack2(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos);
void MakePacket_Attack3(MinLib::StreamBuffer ** packet, BYTE direction, Pos pos);
void SendPacket(MinLib::StreamBuffer * packet, Session * session);