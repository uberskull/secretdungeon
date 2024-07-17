
#include <Windows.h>
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h>
#include <assert.h>

#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler
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
    //Direct3d 11
    ID3D11Device* device_ptr = NULL;
    ID3D11DeviceContext* device_context_ptr = NULL;
    IDXGISwapChain* swap_chain_ptr = NULL;
    ID3D11RenderTargetView* render_target_view_ptr = NULL;

    DXGI_SWAP_CHAIN_DESC swap_chain_descr = { 0 };
    swap_chain_descr.BufferDesc.RefreshRate.Numerator = 0;
    swap_chain_descr.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_descr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_descr.SampleDesc.Count = 1;
    swap_chain_descr.SampleDesc.Quality = 0;
    swap_chain_descr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_descr.BufferCount = 1;
    swap_chain_descr.OutputWindow = hWnd;
    swap_chain_descr.Windowed = true;

    D3D_FEATURE_LEVEL feature_level;
    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
        flags, NULL, 0, D3D11_SDK_VERSION, &swap_chain_descr, &swap_chain_ptr,
        &device_ptr, &feature_level, &device_context_ptr);
    assert(S_OK == hr && swap_chain_ptr && device_ptr && device_context_ptr);

    ID3D11Texture2D* framebuffer;
    hr = swap_chain_ptr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);
    assert(SUCCEEDED(hr));

    hr = device_ptr->CreateRenderTargetView(framebuffer, 0, &render_target_view_ptr);
    assert(SUCCEEDED(hr));
    framebuffer->Release();

    UINT flags2 = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags2 |= D3DCOMPILE_DEBUG; // add more debug output
#endif
    ID3DBlob* vs_blob_ptr = NULL;
    ID3DBlob* ps_blob_ptr = NULL;
    ID3DBlob* error_blob = NULL;

    // COMPILE VERTEX SHADER
    hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vs_main", "vs_5_0", flags2, 0, &vs_blob_ptr, &error_blob);
    if(FAILED(hr))
    {
        if(error_blob)
        {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
            error_blob->Release();
        }
        if(vs_blob_ptr)
        { 
            vs_blob_ptr->Release();
        }
        assert(false);
    }

    // COMPILE PIXEL SHADER
    hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "ps_main", "ps_5_0", flags2, 0, &ps_blob_ptr, &error_blob);
    if(FAILED(hr))
    {
        if(error_blob)
        {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
            error_blob->Release();
        }
        if (ps_blob_ptr)
        {
            ps_blob_ptr->Release();
        }
        assert(false);
    }

    ID3D11VertexShader* vertex_shader_ptr = NULL;
    ID3D11PixelShader* pixel_shader_ptr = NULL;

    hr = device_ptr->CreateVertexShader(vs_blob_ptr->GetBufferPointer(), vs_blob_ptr->GetBufferSize(),
        NULL, &vertex_shader_ptr); 
    assert(SUCCEEDED(hr));

    hr = device_ptr->CreatePixelShader(ps_blob_ptr->GetBufferPointer(), ps_blob_ptr->GetBufferSize(),
        NULL, &pixel_shader_ptr);
    assert(SUCCEEDED(hr));

    ID3D11InputLayout* input_layout_ptr = NULL;
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      /*
      { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      */
    };
    hr = device_ptr->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc),
        vs_blob_ptr->GetBufferPointer(), vs_blob_ptr->GetBufferSize(), &input_layout_ptr);
    assert(SUCCEEDED(hr));

    float vertex_data_array[] =
    {
       0.0f,  0.5f,  0.0f, // point at top
       0.5f, -0.5f,  0.0f, // point at bottom-right
      -0.5f, -0.5f,  0.0f, // point at bottom-left
    };
    UINT vertex_stride = 3 * sizeof(float);
    UINT vertex_offset = 0;
    UINT vertex_count = 3;

    ID3D11Buffer* vertex_buffer_ptr = NULL;
    { /*** load mesh data into vertex buffer **/
        D3D11_BUFFER_DESC vertex_buff_descr = {};
        vertex_buff_descr.ByteWidth = sizeof(vertex_data_array);
        vertex_buff_descr.Usage = D3D11_USAGE_DEFAULT;
        vertex_buff_descr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA sr_data = { 0 };
        sr_data.pSysMem = vertex_data_array;
        HRESULT hr = device_ptr->CreateBuffer(&vertex_buff_descr, &sr_data, &vertex_buffer_ptr);
        assert(SUCCEEDED(hr));
    }

    MSG msg;
    RECT winRect;
    D3D11_VIEWPORT viewport;
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
        /* clear the back buffer to cornflower blue for the new frame */
        float background_colour[4] = 
        {
            0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f
        };
        device_context_ptr->ClearRenderTargetView(render_target_view_ptr, background_colour);

        GetClientRect(hWnd, &winRect);
        viewport =
        {
            0.0f,
            0.0f,
            (FLOAT)(winRect.right - winRect.left),
            (FLOAT)(winRect.bottom - winRect.top),
            0.0f,
            1.0f
        };
        device_context_ptr->RSSetViewports(1, &viewport);
        device_context_ptr->OMSetRenderTargets(1, &render_target_view_ptr, NULL);

        device_context_ptr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device_context_ptr->IASetInputLayout(input_layout_ptr);
        device_context_ptr->IASetVertexBuffers(0, 1, &vertex_buffer_ptr, &vertex_stride, &vertex_offset);

        device_context_ptr->VSSetShader(vertex_shader_ptr, NULL, 0);
        device_context_ptr->PSSetShader(pixel_shader_ptr, NULL, 0);

        device_context_ptr->Draw(vertex_count, 0);
        swap_chain_ptr->Present(1, 0);
    }
    return 0;
}