#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include "Renderer.h"

class Camera
{
public:
	Camera(Renderer* renderer, Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection);
	~Camera();

	void updateViewTransform(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection);

	void updateProjectionTransform();

	DirectX::XMMATRIX getProjectionTransform(){return projectionTransform;}
	DirectX::XMMATRIX getViewTransform(){return viewTransform;}

protected:
	Camera(){renderer = NULL;}
	DirectX::XMMATRIX viewTransform;
	DirectX::XMMATRIX projectionTransform;

	Renderer* renderer;
};

