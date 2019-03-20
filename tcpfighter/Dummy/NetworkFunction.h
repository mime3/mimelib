#pragma once
// 네트워크 세팅 함수
bool Init(SOCKET * listenSocket);
// 섹터 이동의 경우의수 테이블 세팅함수
void TableInit();
// 서버 네트워킹함수
void NetworkingEx();
// 링버퍼에 데이터 수신부
bool RecvProc(SOCKET recvSocket);
// 링버퍼에서 데이터 전송부
void SendProc(SOCKET sendSocket);
// 새 연결시 실행될 함수
bool LoginProcess(SOCKET socket, SOCKADDR_IN addr);
// 연결종료시 실행될 함수
void LogoutProcess(SOCKET socket);
// 링버퍼에서 패킷 1개 빼는 부분
void PacketRecv();
// 패킷 파싱함수
void Parsing(Session * session, WORD message, StreamBuffer * payLoad);
// 네트워킹중 연결종료시 실행될 함수
void CloseSession(Session * session);
// 플레이어의 위치와 인자를 비교하여 데드레커닝 여부를 판별
bool CheckDeadReckoning(Player * player, Pos pos);
// 없어져야할 세션을 실제로 삭제하는 함수
void DeleteSession();