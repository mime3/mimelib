#pragma once
// ���� ���� ����
void Update(HWND hWnd);
// Ű �Է� Ȯ�� �Լ�
void KeyProcess();
// ���� ����
void Logic();
// ������
void Render(HWND hWnd);

// ������ �켱�� ���
int GetPriority(Object * object);
// ���� �˻�
Player * FindUser(int ID);