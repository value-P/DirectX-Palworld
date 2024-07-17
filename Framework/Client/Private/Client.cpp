#include "pch.h"
#include "framework.h"
#include "ClientLoop.h"
#include "Client.h"

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;
HWND g_hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	// 메모리 누수를 감지합니다.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// _crtBreakAlloc = 6843933;

	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance (hInstance, nCmdShow)) { return FALSE; }

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	// 클라이언트 루프를 실행합니다.
	unique_ptr<CClientLoop> upClientLoop = make_unique<CClientLoop>();
	upClientLoop->Initialize();
	upClientLoop->BeginPlay();

	MSG msg{};
	while (true)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message) { break; }

			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			switch (upClientLoop->Tick())
			{
			case 0:		break;
			default:	continue;
			}

			upClientLoop->Render();
		}
	}

	upClientLoop->Release();
	upClientLoop = nullptr;

	_CrtDumpMemoryLeaks();
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = L"Pal World",
	wcex.lpszClassName  = L"Pal World",
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// 인스턴스 핸들을 전역 변수에 저장합니다.
	g_hInst = hInstance;

   RECT tRect{ 0, 0, g_iWindowSizeX, g_iWindowSizeY };
   AdjustWindowRect(&tRect, WS_OVERLAPPEDWINDOW, FALSE);

   HWND hWnd = CreateWindowW
   (
	   L"Pal World",
	   L"Pal World",
	   WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT,
	   0,
	   tRect.right - tRect.left,
	   tRect.bottom - tRect.top,
	   nullptr,
	   nullptr,
	   hInstance,
	   nullptr
   );

   if (!hWnd) { return FALSE; }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _iMessage, WPARAM _wParam, LPARAM _lParam)
{
	switch (_iMessage)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(_wParam);

			switch (wmId)
			{
			case IDM_EXIT:
				DestroyWindow(_hWnd);
				break;

			default:
				return DefWindowProc(_hWnd, _iMessage, _wParam, _lParam);
			}
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(_hWnd, &ps);
			EndPaint(_hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(_hWnd, _iMessage, _wParam, _lParam);
	}

	return 0;
}
