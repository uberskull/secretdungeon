
#include <Windows.h>
#include <d3d11.h>
#include <memory>
#include <assert.h>

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
    //Direct3d varibles
    ID3D11Device* pDevice = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;

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
    HWND hWnd = CreateWindowEx(0, pClassName, pWindowName, WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
    200, 200, 640, 480,
    nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hWnd, SW_SHOW);

    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    //Swap chain config
    DXGI_SWAP_CHAIN_DESC swapChainDescr = {};
    swapChainDescr.BufferDesc.Width = 0;
    swapChainDescr.BufferDesc.Height = 0;
    swapChainDescr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDescr.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDescr.BufferDesc.RefreshRate.Denominator = 0;
    swapChainDescr.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDescr.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDescr.SampleDesc.Count = 1;
    swapChainDescr.SampleDesc.Quality = 0;
    swapChainDescr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescr.BufferCount = 1;
    swapChainDescr.OutputWindow = hWnd;
    swapChainDescr.Windowed = TRUE;
    swapChainDescr.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDescr.Flags = flags;
    //Create swap chain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDescr,
        &pSwap, &pDevice, nullptr, &pContext);
    assert(SUCCEEDED(hr));

    ID3D11Resource* pBackBuffer = nullptr;
    hr = pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
    assert(SUCCEEDED(hr));

    hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
    assert(SUCCEEDED(hr));

    pBackBuffer->Release();
    const float color[] = { 1.0, 0.0, 0.0, 1.0f };

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
        //Clear with color
        pContext->ClearRenderTargetView(pTarget, color);
        //Buffer swap
        pSwap->Present(1u, 0u);
    }
    pTarget->Release();
    pSwap->Release();
    pContext->Release();
    pDevice->Release();
    
    return 0;
}