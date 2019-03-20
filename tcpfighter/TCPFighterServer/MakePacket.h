#pragma once

void MakePacket_Create_My_Character(StreamBuffer ** packet, int ID, BYTE direction, Pos pos, BYTE HP);
void MakePacket_Create_Other_Character(StreamBuffer ** packet, int ID, BYTE direction, Pos pos, BYTE HP);
void MakePacket_Delete_Character(StreamBuffer ** packet, int ID);
void MakePacket_Move_Start(StreamBuffer ** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Move_Stop(StreamBuffer ** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Attack1(StreamBuffer ** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Attack2(StreamBuffer ** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Attack3(StreamBuffer ** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Damage(StreamBuffer ** packet, int AttackID, int DamageID, BYTE damageHP);
void MakePacket_Sync(StreamBuffer ** packet, int ID, Pos pos);
void MakePacket_Echo(StreamBuffer ** packet, DWORD time);

void UniCast(StreamBuffer * stream, Session * session, bool deletePacket = true);
void BroadCast_All(StreamBuffer * stream, Session * session, bool dontSend = true);
void BroadCast_Sector(StreamBuffer * stream, Session * session, bool dontSend = true);