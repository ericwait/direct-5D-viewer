#include "DepthTarget.h"

RenderDepthTarget::RenderDepthTarget(Renderer* renderer, size_t width, size_t height)
	: renderer(renderer), dims(Vec<size_t>(width,height,1)), depthTarget(NULL)
{
	initDepthTarget();	
}

void RenderDepthTarget::initDepthTarget()
{
	D3D11_TEXTURE2D_DESC desc;

	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.Width = dims.x;
	desc.Height = dims.y;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D* depthTexture = renderer->createTexture2D(&desc, NULL);

	depthTarget = renderer->createDepthTargetView(depthTexture);

	SAFE_RELEASE(depthTexture);
}

void RenderDepthTarget::resizeDepth(size_t width, size_t height)
{
	Vec<size_t> newDims(width, height, 1);
	if ( newDims == dims )
		return;

	dims = newDims;

	SAFE_RELEASE(depthTarget);
	initDepthTarget();
}

RenderDepthTarget::~RenderDepthTarget()
{
	SAFE_RELEASE(depthTarget);
}
