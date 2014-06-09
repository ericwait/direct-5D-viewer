#include "Camera.h"
#include "Globals.h"

Camera::Camera(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection)
{
	updateViewTransform(cameraPostion,lookPostion,upDirection);
	updateProjectionTransform();
}

void Camera::updateViewTransform(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection)
{
	DirectX::XMFLOAT3 eye(cameraPostion.x,cameraPostion.y,cameraPostion.z);
	DirectX::FXMVECTOR eyeVec = DirectX::XMLoadFloat3(&eye);
	DirectX::XMFLOAT3 focus(lookPostion.x,lookPostion.y,lookPostion.z);
	DirectX::FXMVECTOR focusVec = DirectX::XMLoadFloat3(&focus);
	DirectX::XMFLOAT3 up(upDirection.x,upDirection.y,upDirection.z);
	DirectX::FXMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	viewTransform = DirectX::XMMatrixLookAtRH(eyeVec,focusVec,upVec);
}

void Camera::updateProjectionTransform()
{
	projectionTransform = DirectX::XMMatrixPerspectiveFovRH(DirectX::XM_PI/4.0f, (float)gWindowWidth/gWindowHeight, 0.01f, 100.0f);
}



OrthoCamera::OrthoCamera(Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection) : Camera(cameraPostion,lookPostion,upDirection)
{
	updateViewTransform(cameraPostion,lookPostion,upDirection);
	updateProjectionTransform();
}

void OrthoCamera::updateProjectionTransform()
{
	float orthoHeight = 2.0;
	float aspectRatio = ((FLOAT)gWindowWidth/(FLOAT)gWindowHeight);

	float widgetPixSize = 40.0;
	float widgetPixSpacing = 5.0;

	float widgetScale = widgetPixSize / ((float)gWindowHeight / 2.0);
	float ctrY = ((widgetPixSize + widgetPixSpacing) / ((float)gWindowHeight / 2.0)) - 1.0;
	float ctrX = ((widgetPixSize + widgetPixSpacing) / ((float)gWindowWidth / 2.0)) - 1.0;

	projectionTransform = DirectX::XMMatrixOrthographicRH(aspectRatio*orthoHeight, orthoHeight, 0.01f, 100.0f)
		* DirectX::XMMatrixScaling(widgetScale, widgetScale, 1.0f) * DirectX::XMMatrixTranslation(ctrX, ctrY, 0.0f);
}
