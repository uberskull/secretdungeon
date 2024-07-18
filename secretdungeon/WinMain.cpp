
#include <Windows.h>

#include <memory>

#include "Graphics.hpp"

//#include <winuser.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(msg == WM_CLOSE)
    {
        PostQuitMessage(0);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, 
_In_ int nCmdShow)
{
    LPCWSTR pClassName = L"class";
    LPCWSTR pWindowName = L"window";
    bool running = true;

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = pClassName;
    wc.hIconSm = nullptr;
    RegisterClassEx(&wc);
    
    //Create window instance
    HWND hWnd = CreateWindowEx(0, pClassName, pWindowName,
    WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
    200, 200, 640, 480,
    nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hWnd, SW_SHOW);
    //Direct 3d
    std::unique_ptr<Graphics> pGfx = std::make_unique<Graphics>(hWnd);

    MSG msg;
    //Main loop
    while (running == true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        pGfx->clearBuffer(1.0, 0.0, 0.0);
        pGfx->EndFrame();
    }
    return 0;
}