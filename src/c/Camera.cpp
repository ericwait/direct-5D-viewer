#include "Camera.h"
#include "Globals.h"

Camera::Camera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn)
{
	cameraPosition = defaultCameraPosition = cameraPositionIn;
	lookPosition = defaultLookPosition = lookPositionIn;
	upDirection = defaultUpDirection = upDirectionIn;

	updateViewTransform();
	updateProjectionTransform();
}

void Camera::moveLeft()
{
	cameraPosition.x -= 0.1f;
	lookPosition.x	 -= 0.1f;
	updateViewTransform();
}

void Camera::moveRight()
{
	cameraPosition.x += 0.1f;
	lookPosition.x	 += 0.1f;
	updateViewTransform();
}

void Camera::moveUp()
{
	cameraPosition.y -= 0.1f;
	lookPosition.y	 -= 0.1f;
	updateViewTransform();
}

void Camera::moveDown()
{
	cameraPosition.y += 0.1f;
	lookPosition.y	 += 0.1f;
	updateViewTransform();
}

void Camera::zoomIncrement()
{
	if (lookPosition.z-cameraPosition.z>0.02f)
	{
		cameraPosition.z += (lookPosition.z-cameraPosition.z)*0.125f;
		updateViewTransform();
	}
}

void Camera::zoomDecrement()
{
	if (lookPosition.z-cameraPosition.z<10.0f)
	{
		cameraPosition.z -= (lookPosition.z-cameraPosition.z)*0.25f;
		updateViewTransform();
	}
}


void Camera::resetCamera()
{
	cameraPosition = defaultCameraPosition;
	lookPosition = defaultLookPosition;
	upDirection = defaultUpDirection;
	updateViewTransform();
}

void Camera::setCameraPosition(Vec<float> cameraPositionIn)
{
	cameraPosition = cameraPositionIn;
	updateViewTransform();
}

void Camera::setLookPosition(Vec<float> lookPositionIn)
{
	lookPosition = lookPositionIn;
	updateViewTransform();
}

void Camera::setUpDirection(Vec<float> upDirectionIn)
{
	upDirection = upDirectionIn;
	updateViewTransform();
}

void Camera::updateProjectionTransform()
{
	projectionTransform = DirectX::XMMatrixPerspectiveFovRH(DirectX::XM_PI/4.0f, (float)gWindowWidth/gWindowHeight, 0.2f, 25.0f);
}


void Camera::setCamera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn)
{
	cameraPosition = cameraPositionIn;
	lookPosition = lookPositionIn;
	upDirection = upDirectionIn;

	updateViewTransform();
}


void Camera::updateViewTransform()
{
	DirectX::XMFLOAT3 eye(cameraPosition.x,cameraPosition.y,cameraPosition.z);
	DirectX::FXMVECTOR eyeVec = DirectX::XMLoadFloat3(&eye);
	DirectX::XMFLOAT3 focus(lookPosition.x,lookPosition.y,lookPosition.z);
	DirectX::FXMVECTOR focusVec = DirectX::XMLoadFloat3(&focus);
	DirectX::XMFLOAT3 up(upDirection.x,upDirection.y,upDirection.z);
	DirectX::FXMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	viewTransform = DirectX::XMMatrixLookAtRH(eyeVec,focusVec,upVec);
}



OrthoCamera::OrthoCamera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn)
	: Camera(cameraPositionIn,lookPositionIn,upDirectionIn)
{
	updateViewTransform();
	updateProjectionTransform();
}

void OrthoCamera::updateProjectionTransform()
{
	float orthoHeight = 2.0;
	float aspectRatio = ((FLOAT)gWindowWidth/(FLOAT)gWindowHeight);

	float widgetPixSize = 40.0;
	float widgetPixSpacing = 5.0;

	float widgetScale = widgetPixSize / ((float)gWindowHeight / 2.0f);
	float ctrY = ((widgetPixSize + widgetPixSpacing) / ((float)gWindowHeight / 2.0f)) - 1.0f;
	float ctrX = ((widgetPixSize + widgetPixSpacing) / ((float)gWindowWidth / 2.0f)) - 1.0f;

	projectionTransform = DirectX::XMMatrixOrthographicRH(aspectRatio*orthoHeight, orthoHeight, 0.01f, 100.0f)
		* DirectX::XMMatrixScaling(widgetScale, widgetScale, 1.0f) * DirectX::XMMatrixTranslation(ctrX, ctrY, 0.0f);
}
