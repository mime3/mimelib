#pragma once
// ��Ʈ��ũ ���� �Լ�
bool Init(SOCKET * listenSocket);
// ���� �̵��� ����Ǽ� ���̺� �����Լ�
void TableInit();
// ���� ��Ʈ��ŷ�Լ�
void NetworkingEx();
// �����ۿ� ������ ���ź�
bool RecvProc(SOCKET recvSocket);
// �����ۿ��� ������ ���ۺ�
void SendProc(SOCKET sendSocket);
// �� ����� ����� �Լ�
bool LoginProcess(SOCKET socket, SOCKADDR_IN addr);
// ��������� ����� �Լ�
void LogoutProcess(SOCKET socket);
// �����ۿ��� ��Ŷ 1�� ���� �κ�
void PacketRecv();
// ��Ŷ �Ľ��Լ�
void Parsing(Session * session, WORD message, StreamBuffer * payLoad);
// ��Ʈ��ŷ�� ��������� ����� �Լ�
void CloseSession(Session * session);
// �÷��̾��� ��ġ�� ���ڸ� ���Ͽ� ���巹Ŀ�� ���θ� �Ǻ�
bool CheckDeadReckoning(Player * player, Pos pos);
// ���������� ������ ������ �����ϴ� �Լ�
void DeleteSession();