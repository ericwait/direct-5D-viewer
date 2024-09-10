#include "RenderTarget.h"


const DXGI_FORMAT SwapChainTarget::format = DXGI_FORMAT_B8G8R8A8_UNORM;
const UINT SwapChainTarget::chainFlags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE
										| DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


SwapChainTarget::SwapChainTarget(Renderer* rendererIn, HWND hWnd, size_t width, size_t height)
	: renderer(rendererIn), dims(Vec<size_t>(width,height,1)), renderTarget(NULL)
{
	swapChain = renderer->createSwapChain(hWnd, dims, false, format, chainFlags);
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



ReadbackRenderTarget::ReadbackRenderTarget(Renderer* rendererIn, size_t width, size_t height)
	: renderer(rendererIn), dims(Vec<size_t>(width, height, 1)), renderSurface(NULL), stagingSurface(NULL), renderTarget(NULL)
{
	bool created = createBuffers();
	if ( !created )
		std::runtime_error("Unable to create readback texture!");
}

ReadbackRenderTarget::~ReadbackRenderTarget()
{
	clearBuffers();
}

bool ReadbackRenderTarget::clearBuffers()
{
	SAFE_RELEASE(renderSurface);
	SAFE_RELEASE(stagingSurface);

	SAFE_RELEASE(renderTarget);

	return true;
}

bool ReadbackRenderTarget::createBuffers()
{
	UINT iMipCount = 1;
	UINT BitSize = 0;

	// Shared description info
	D3D11_TEXTURE2D_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = (unsigned int)dims.x;
	desc.Height = (unsigned int)dims.y;
	desc.MipLevels = iMipCount;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Render-texture descriptor
	D3D11_TEXTURE2D_DESC renderDesc(desc);
	renderDesc.Usage = D3D11_USAGE_DEFAULT;
	renderDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	renderDesc.CPUAccessFlags = 0;

	// Readback staging texture descriptor
	D3D11_TEXTURE2D_DESC stagingDesc(desc);
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;


	renderSurface = renderer->createTexture2D(&renderDesc, NULL);
	if ( !renderSurface )
		return false;

	renderTarget = renderer->createRenderTargetView(renderSurface);
	if ( !renderTarget )
		return false;

	stagingSurface = renderer->createTexture2D(&stagingDesc, NULL);
	if ( !stagingSurface )
		return false;

	return true;
}

void ReadbackRenderTarget::resizeTarget(size_t width, size_t height)
{
	dims = Vec<size_t>(width, height, 1);
	clearBuffers();
	createBuffers();
}

unsigned char* ReadbackRenderTarget::capture()
{
	unsigned char* outData = new unsigned char[4*dims.product()];

	renderer->stageResource(stagingSurface, renderSurface);
	renderer->readSurface(outData, stagingSurface, dims, 4);

	return outData;
}
