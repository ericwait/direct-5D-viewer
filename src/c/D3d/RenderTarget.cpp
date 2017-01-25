#include "RenderTarget.h"


const DXGI_FORMAT SwapChainTarget::format = DXGI_FORMAT_B8G8R8A8_UNORM;
const UINT SwapChainTarget::chainFlags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE
										| DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


SwapChainTarget::SwapChainTarget(Renderer* rendererIn, HWND hWnd, size_t width, size_t height)
	: renderer(rendererIn), dims(Vec<size_t>(width,height,1)), renderTarget(NULL)
{
	swapChain = renderer->createSwapChain(hWnd, dims, format, chainFlags);
	initRenderTarget();
}

void SwapChainTarget::initRenderTarget()
{
	HRESULT hr;

	ID3D11Texture2D* backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if( FAILED(hr) )
		std::runtime_error("Unable to get swap-chain back buffer!");

	hr = backBuffer->QueryInterface(__uuidof(IDXGISurface1), (LPVOID*)&backBufferSurface);
	if( FAILED(hr) )
		std::runtime_error("Could not create GDI compatible backbuffer resource!");

	// Create a render target view using the swapchain's back buffer
	renderTarget = renderer->createRenderTargetView(backBuffer);

	SAFE_RELEASE(backBuffer);
}

void SwapChainTarget::resizeTarget(size_t width, size_t height)
{
	Vec<size_t> newDims(width, height, 1);
	if ( newDims == dims )
		return;

	dims = newDims;

	// Release all direct and indirect references to backbuffers
	SAFE_RELEASE(renderTarget);
	SAFE_RELEASE(backBufferSurface);

	swapChain->ResizeBuffers(2, dims.x, dims.y, format, chainFlags);

	// Reinitialize render target view and backbuffer surface
	initRenderTarget();
}

HDC SwapChainTarget::getDC() const
{
	HDC hdc;
	backBufferSurface->GetDC(FALSE, &hdc);

	return hdc;
}

void SwapChainTarget::releaseDC() const
{
	backBufferSurface->ReleaseDC(NULL);
}

void SwapChainTarget::present(UINT syncInterval, UINT flags) const
{
	swapChain->Present(syncInterval, flags);
}



SwapChainTarget::~SwapChainTarget()
{
	SAFE_RELEASE(renderTarget);
	SAFE_RELEASE(backBufferSurface);
	SAFE_RELEASE(swapChain);
}
