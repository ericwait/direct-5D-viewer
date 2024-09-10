#pragma once

#include "Renderer.h"
#include "Texture.h"

class DepthTarget
{
public:
	virtual ID3D11DepthStencilView* getDepthTarget() = 0;
	virtual void resizeDepth(size_t width, size_t height) = 0;

	virtual ~DepthTarget(){}
};

class RenderDepthTarget : public DepthTarget
{
public:
	RenderDepthTarget(Renderer* renderer, size_t width, size_t height);

	virtual ID3D11DepthStencilView* getDepthTarget(){return depthTarget;}
	virtual void resizeDepth(size_t width, size_t height);
	virtual ~RenderDepthTarget();

private:
	void initDepthTarget();


	Renderer* renderer = nullptr;
	Vec<size_t> dims = Vec<size_t>(0,0,0);

	ID3D11DepthStencilView* depthTarget;
};