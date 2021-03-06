// main.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "main.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

ScreenDIB *screen;
SpriteDIB *sprite;
FrameSkip *frameSkip;
CList<Object *> g_list;
Player * g_player;
TileMap * g_tileMap;
HWND hWnd;
bool active;

#ifdef LOG
//LOG
FILE *file;
#endif
// IMC
HIMC oldIMC;

// network
WCHAR g_IP[16];
Session mySession;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_ADDR), NULL, DialogProc);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TCPFIGHTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TCPFIGHTER));

    MSG msg;

    // 기본 메시지 루프입니다.
	while (1)
	{
		if(PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
		    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		    {
		        TranslateMessage(&msg);
		        DispatchMessage(&msg);
		    }
		}
		else
		{
			Update(hWnd);
		}
	}

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCPFIGHTER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TCPFIGHTER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   RECT windowRect = { 0,0,SCREEN_X,SCREEN_Y };
   AdjustWindowRectEx(&windowRect, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd));
   int x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (SCREEN_X / 2);
   int y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (SCREEN_Y / 2);

   MoveWindow(hWnd, x, y, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, TRUE);

   ShowWindow(hWnd, nCmdShow);
   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.d
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		oldIMC = ImmAssociateContext(hWnd, NULL);

		//Map * map = new Map(Pos{0, 0});
		g_tileMap = new TileMap(100, 100);
		frameSkip = new FrameSkip(50);

		//	캐릭터 스프라이트 - 71, 90
		//	이펙트 스프라이트 - 70, 70
		//	HP 게이지 - 0, 0
		//	그림자 - 32, 4
		screen = new ScreenDIB(SCREEN_X, SCREEN_Y, 32);
		sprite = new SpriteDIB(SPRITE_MAX, 0xffffffff);
		sprite->LoadDIBSprite(MAP, "./sprite/_Map.bmp", 0, 0);
		sprite->LoadDIBSprite(PLAYER_STAND_L01, "./sprite/Stand_L_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_STAND_L02, "./sprite/Stand_L_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_STAND_L03, "./sprite/Stand_L_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_STAND_R01, "./sprite/Stand_R_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_STAND_R02, "./sprite/Stand_R_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_STAND_R03, "./sprite/Stand_R_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L01, "./sprite/Move_L_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L02, "./sprite/Move_L_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L03, "./sprite/Move_L_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L04, "./sprite/Move_L_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L05, "./sprite/Move_L_05.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L06, "./sprite/Move_L_06.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L07, "./sprite/Move_L_07.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L08, "./sprite/Move_L_08.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L09, "./sprite/Move_L_09.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L10, "./sprite/Move_L_10.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L11, "./sprite/Move_L_11.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_L12, "./sprite/Move_L_12.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R01, "./sprite/Move_R_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R02, "./sprite/Move_R_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R03, "./sprite/Move_R_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R04, "./sprite/Move_R_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R05, "./sprite/Move_R_05.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R06, "./sprite/Move_R_06.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R07, "./sprite/Move_R_07.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R08, "./sprite/Move_R_08.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R09, "./sprite/Move_R_09.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R10, "./sprite/Move_R_10.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R11, "./sprite/Move_R_11.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_MOVE_R12, "./sprite/Move_R_12.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_L01, "./sprite/Attack1_L_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_L02, "./sprite/Attack1_L_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_L03, "./sprite/Attack1_L_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_L04, "./sprite/Attack1_L_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_R01, "./sprite/Attack1_R_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_R02, "./sprite/Attack1_R_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_R03, "./sprite/Attack1_R_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK1_R04, "./sprite/Attack1_R_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_L01, "./sprite/Attack2_L_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_L02, "./sprite/Attack2_L_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_L03, "./sprite/Attack2_L_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_L04, "./sprite/Attack2_L_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_R01, "./sprite/Attack2_R_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_R02, "./sprite/Attack2_R_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_R03, "./sprite/Attack2_R_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK2_R04, "./sprite/Attack2_R_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_L01, "./sprite/Attack3_L_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_L02, "./sprite/Attack3_L_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_L03, "./sprite/Attack3_L_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_L04, "./sprite/Attack3_L_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_L05, "./sprite/Attack3_L_05.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_L06, "./sprite/Attack3_L_06.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_R01, "./sprite/Attack3_R_01.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_R02, "./sprite/Attack3_R_02.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_R03, "./sprite/Attack3_R_03.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_R04, "./sprite/Attack3_R_04.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_R05, "./sprite/Attack3_R_05.bmp", 71, 90);
		sprite->LoadDIBSprite(PLAYER_ATTACK3_R06, "./sprite/Attack3_R_06.bmp", 71, 90);
		sprite->LoadDIBSprite(EFFECT_SPARK_01, "./sprite/xSpark_1.bmp", 70, 70);
		sprite->LoadDIBSprite(EFFECT_SPARK_02, "./sprite/xSpark_2.bmp", 70, 70);
		sprite->LoadDIBSprite(EFFECT_SPARK_03, "./sprite/xSpark_3.bmp", 70, 70);
		sprite->LoadDIBSprite(EFFECT_SPARK_04, "./sprite/xSpark_4.bmp", 70, 70);
		sprite->LoadDIBSprite(GUAGE_HP, "./sprite/HPGuage.bmp", 0, 0);
		sprite->LoadDIBSprite(SHADOW, "./sprite/Shadow.bmp", 32, 4);
		sprite->LoadDIBSprite(TILE, "./sprite/Tile_01.bmp", 0, 0);

		WsaInit(hWnd);
#ifdef LOG
		char fileName[16];
		sprintf_s(fileName, "Log/*%d_%d*/.txt");
		freopen_s(&file,"Log.txt", "w", stdout);
		printf_s("LogFile Init\n");
#endif 
		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_ACTIVATE:
	{
		switch(LOWORD(wParam))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			active = true;
			break;
		case WA_INACTIVE:
			active = false;
			break;
		}
		break;
	}
	case UM_NETWORK:
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT:
			mySession.connected = true;
			mySession.sendFlag = true;
			break;
		case FD_READ:
			ProcRead();
			break;
		case FD_WRITE:
			ProcWrite();
			break;
		case FD_CLOSE:
			ProcClose();
			break;
		}
		break;
	}
    case WM_DESTROY:
#ifdef LOG
		printf_s("Log End\n");
		fclose(stdout);
#endif
		ImmAssociateContext(hWnd, oldIMC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		memset(g_IP, 0, sizeof(WCHAR) * 16);
		HWND editBox;
		editBox = GetDlgItem(hDlg, IDC_IPADDR);
		SetWindowText(editBox, L"127.0.0.1");
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_IPADDR, g_IP, 16);
			EndDialog(hDlg, 99939);
			return (INT_PTR)TRUE;
		}
		return (INT_PTR)FALSE;
	}
	return (INT_PTR)FALSE;
}