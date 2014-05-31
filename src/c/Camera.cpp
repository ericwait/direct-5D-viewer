#include "Camera.h"
#include "Globals.h"

Camera::Camera(Renderer* renderer, Vec<float> cameraPostion, Vec<float> lookPostion, Vec<float> upDirection)
{
	DirectX::XMFLOAT3 eye(cameraPostion.x,cameraPostion.y,cameraPostion.z);
	DirectX::FXMVECTOR eyeVec = DirectX::XMLoadFloat3(&eye);
	DirectX::XMFLOAT3 focus(lookPostion.x,lookPostion.y,lookPostion.z);
	DirectX::FXMVECTOR focusVec = DirectX::XMLoadFloat3(&focus);
	DirectX::XMFLOAT3 up(upDirection.x,upDirection.y,upDirection.z);
	DirectX::FXMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	viewTransform = DirectX::XMMatrixLookAtRH(eyeVec,focusVec,upVec);

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
