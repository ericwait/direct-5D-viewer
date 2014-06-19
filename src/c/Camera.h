#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include "Renderer.h"

class Camera
{
public:
	Camera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn);

	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void zoomIncrement();
	void zoomDecrement();
	void resetCamera();
	void setCameraPosition(Vec<float> cameraPositionIn);
	void setLookPosition(Vec<float> lookPositionIn);
	void setUpDirection(Vec<float> upDirectionIn);
	void setCamera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn);
	virtual void updateProjectionTransform();

	DirectX::XMMATRIX getProjectionTransform() const {return projectionTransform;}
	DirectX::XMMATRIX getViewTransform() const {return viewTransform;}

protected:
	Camera(){}

	virtual void updateViewTransform();
	Vec<float> cameraPosition;
	Vec<float> lookPosition;
	Vec<float> upDirection;
	Vec<float> defaultCameraPosition;
	Vec<float> defaultLookPosition;
	Vec<float> defaultUpDirection;
	DirectX::XMMATRIX viewTransform;
	DirectX::XMMATRIX projectionTransform;
	float zoomFactor;
};


class OrthoCamera : public Camera
{
public:
	OrthoCamera(Vec<float> cameraPostionIn, Vec<float> lookPostionIn, Vec<float> upDirectionIn);

	void updateProjectionTransform();

private:
	OrthoCamera();
};
