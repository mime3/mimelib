#pragma once
// 네트워크 초기세팅
void WsaInit(HWND hWnd);
// 링버퍼에서 패킷 꺼내기
void PacketProc();
// 최초 연결성공시 호출될 프로시저
void ProcConnect();
// 데이터 수신시 호출될 프로시저
void ProcRead();
// 데이터 송신가능시 호출될 프로시저
void ProcWrite();
// 연결이 종료됬을때 호출될 프로시저
void ProcClose();
// 패킷을 링버퍼에 넣고, 링버퍼의 데이터를 모두 send시도
void SendPacket(char * buffer, int size);
// 수신된 패킷 파싱
void PacketParse(BYTE packetType, MinLib::StreamBuffer &payload);

// 패킷 생성 및 전송
void MakeSendPacket(TAG_ACTION action);
void MakeSendMovePacket(BYTE direction, WORD x, WORD y);
void MakeSendAttackPacket(int type, BYTE direction, WORD x, WORD y);
void MakeSendStandPacket(BYTE direction, WORD x, WORD y);