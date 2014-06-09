#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include "Renderer.h"

class Camera
{
public:
	Camera(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection);

	virtual void updateViewTransform(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection);

	virtual void updateProjectionTransform();

	DirectX::XMMATRIX getProjectionTransform() const {return projectionTransform;}
	DirectX::XMMATRIX getViewTransform() const {return viewTransform;}

protected:
	Camera(){}
	DirectX::XMMATRIX viewTransform;
	DirectX::XMMATRIX projectionTransform;
};


class OrthoCamera : public Camera
{
public:
	OrthoCamera(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection);

	void updateProjectionTransform();

private:
	OrthoCamera();
};
