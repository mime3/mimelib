#pragma once
// ��Ʈ��ũ �ʱ⼼��
void WsaInit(HWND hWnd);
// �����ۿ��� ��Ŷ ������
void PacketProc();
// ���� ���Ἲ���� ȣ��� ���ν���
void ProcConnect();
// ������ ���Ž� ȣ��� ���ν���
void ProcRead();
// ������ �۽Ű��ɽ� ȣ��� ���ν���
void ProcWrite();
// ������ ��������� ȣ��� ���ν���
void ProcClose();
// ��Ŷ�� �����ۿ� �ְ�, �������� �����͸� ��� send�õ�
void SendPacket(char * buffer, int size);
// ���ŵ� ��Ŷ �Ľ�
void PacketParse(BYTE packetType, StreamBuffer &payload);

// ��Ŷ ���� �� ����
void MakeSendPacket(TAG_ACTION action);
void MakeSendMovePacket(BYTE direction, WORD x, WORD y);
void MakeSendAttackPacket(int type, BYTE direction, WORD x, WORD y);
void MakeSendStandPacket(BYTE direction, WORD x, WORD y);