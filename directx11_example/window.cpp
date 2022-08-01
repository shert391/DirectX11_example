#include <Windows.h>
#include "directx.h"
#include "window.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

HRESULT Window::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcex{};

	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"d3d";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);

    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    this->hInstance = hInstance;

    RECT rc = { 0, 0, 640, 480 };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindow(L"d3d", L"DirectX11", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

    if (!hwnd)
        return E_FAIL;

    ShowWindow(hwnd, nCmdShow);

    return S_OK;
}

int Window::Load(HINSTANCE hInstance, int nCmdShow)
{
    Directx directx{};

    if (FAILED(InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(directx.InitD3D(hwnd)))
    {
        directx.CleanupDevice();
        return 0;
    }

	if (FAILED(directx.InitPipeline()))
	{
		directx.CleanupDevice();
		return 0;
	}

    directx.InitMatrix();

    MSG msg = { 0 };

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			directx.SetMatrix();
            directx.Render();
        }
    }

    directx.CleanupDevice();

    return (int)msg.wParam;
}
