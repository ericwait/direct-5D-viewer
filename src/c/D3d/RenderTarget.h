#pragma once

#include "Renderer.h"
#include "Texture.h"

class RenderTarget
{
public:
	virtual ID3D11RenderTargetView* getRenderTarget() = 0;
	virtual void resizeTarget(size_t width, size_t height) = 0;

	virtual ~RenderTarget(){}
};


class SwapChainTarget : public RenderTarget
{
public:
	SwapChainTarget(Renderer* rendererIn, HWND hWnd, size_t width, size_t height);

	virtual ID3D11RenderTargetView* getRenderTarget(){return renderTarget;}
	virtual void resizeTarget(size_t width, size_t height);
	virtual ~SwapChainTarget();

	HDC getDC() const;
	void releaseDC() const;

	void present(UINT syncInterval, UINT flags) const;

private:
	void initRenderTarget();

	// Swap chain creation flags
	static const UINT chainFlags;
	static const DXGI_FORMAT format;

	Renderer* renderer;
	Vec<size_t> dims;

	ID3D11RenderTargetView* renderTarget;

	IDXGISwapChain1* swapChain;
	IDXGISurface1* backBufferSurface;
};
