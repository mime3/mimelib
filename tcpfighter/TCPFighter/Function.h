#pragma once
// 게임 메인 루프
void Update(HWND hWnd);
// 키 입력 확인 함수
void KeyProcess();
// 게임 로직
void Logic();
// 렌더링
void Render(HWND hWnd);

// 렌더링 우선도 계산
int GetPriority(Object * object);
// 유저 검색
Player * FindUser(int ID);