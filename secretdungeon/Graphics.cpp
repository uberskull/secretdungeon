
#include "Graphics.hpp"

Graphics::Graphics(HWND hWnd)
{
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
	swapChainDescr.Flags = 0;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDescr,
		&pSwap, &pDevice, nullptr, &pContext);

	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	if(pBackBuffer != nullptr)
	{
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
		pBackBuffer->Release();
	}
}

Graphics::~Graphics()
{
	if(pTarget != nullptr)
		pTarget->Release();
	if(pSwap != nullptr)
		pSwap->Release();
	if(pContext != nullptr)
		pContext->Release();
	if(pDevice != nullptr)
		pDevice->Release();
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}

void Graphics::clearBuffer(float red, float green, float blue)
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}
