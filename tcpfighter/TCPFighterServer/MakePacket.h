#pragma once
void MakePacket_Create_My_Character(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos, BYTE HP);
void MakePacket_Create_Other_Character(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos, BYTE HP);
void MakePacket_Delete_Character(MinLib::StreamBuffer** packet, int ID);
void MakePacket_Move_Start(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Move_Stop(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Attack1(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Attack2(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Attack3(MinLib::StreamBuffer** packet, int ID, BYTE direction, Pos pos);
void MakePacket_Damage(MinLib::StreamBuffer** packet, int AttackID, int DamageID, BYTE damageHP);
void MakePacket_Sync(MinLib::StreamBuffer** packet, int ID, Pos pos);
void MakePacket_Echo(MinLib::StreamBuffer** packet, DWORD time);

void UniCast(MinLib::StreamBuffer* stream, Session * session, bool deletePacket = true);
void BroadCast_All(MinLib::StreamBuffer* stream, Session * session, bool dontSend = true);
void BroadCast_Sector(MinLib::StreamBuffer* stream, Session * session, bool dontSend = true);