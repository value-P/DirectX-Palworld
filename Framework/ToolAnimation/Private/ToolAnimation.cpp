#include "pch.h"
#include "framework.h"
#include "ToolAnimationLoop.h"
#include "ToolAnimation.h"

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;
HWND g_hWnd;
float g_fTimeScale = 1.f;
int g_iNotifyID = 0;

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

    //_CrtSetBreakAlloc(239171);

    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow)) { return FALSE; }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOOLANIMATION));

    // 클라이언트 루프를 실행합니다.
    unique_ptr<ToolAnimationLoop> upToolAnimationLoop = make_unique<ToolAnimationLoop>();
    upToolAnimationLoop->Initialize();
    upToolAnimationLoop->BeginPlay();

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
            switch (upToolAnimationLoop->Tick())
            {
            case 0:		break;
            default:	continue;
            }

            upToolAnimationLoop->Render();
        }
    }

    upToolAnimationLoop->Release();
    upToolAnimationLoop = nullptr;

    _CrtDumpMemoryLeaks();
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOOLANIMATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TOOLANIMATION);
    wcex.lpszMenuName = L"Pal World",
    wcex.lpszClassName = L"Pal World",
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) { return true; }

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED) { return 0; }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) { return 0; }
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;

    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}