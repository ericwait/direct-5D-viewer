////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "Camera.h"
#include "Global/Globals.h"

Camera::Camera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn)
{
	cameraPosition = defaultCameraPosition = cameraPositionIn;
	lookPosition = defaultLookPosition = lookPositionIn;
	upDirection = defaultUpDirection = upDirectionIn;

	nearZ = 0.1f;

	zoomFactor = 25;

	updateViewTransform();
	updateProjectionTransform();
}

void Camera::move(Vec<float> delta)
{
	// TODO: Should this be in here?
	const float panFactor = 0.03;

	cameraPosition += delta * panFactor;
	lookPosition += delta * panFactor;

	updateViewTransform();
}

void Camera::setNearZ(float val)
{
	nearZ = MAX(val,0.0f);
	updateProjectionTransform();
}

void Camera::resetCamera()
{
	cameraPosition = defaultCameraPosition;
	lookPosition = defaultLookPosition;
	upDirection = defaultUpDirection;
	nearZ = 0.1f;

	updateViewTransform();
	updateProjectionTransform();
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
	projectionTransform = ConvertMatrix(DirectX::XMMatrixPerspectiveFovRH(DirectX::XM_PI/4.0f, (float)gWindowWidth/gWindowHeight, nearZ, 25.0f));
}


void Camera::setCamera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn)
{
	cameraPosition = cameraPositionIn;
	lookPosition = lookPositionIn;
	upDirection = upDirectionIn;

	updateViewTransform();
}

void Camera::getRay(int iMouseX, int iMouseY, Vec<float>& pointOut, Vec<float>& directionOut)
{
	DirectX::XMFLOAT3 getVal(1,0,0);
	DirectX::XMVECTOR getVec = DirectX::XMLoadFloat3(&getVal);
	DirectX::XMVECTOR vals = DirectX::XMVector3TransformNormal(getVec,ConvertMatrix(projectionTransform));
	float widthDiv = DirectX::XMVectorGetX(vals);

	getVal = DirectX::XMFLOAT3(0,1,0);
	getVec = DirectX::XMLoadFloat3(&getVal);
	vals = DirectX::XMVector3TransformNormal(getVec, ConvertMatrix(projectionTransform));
	float heightDiv = DirectX::XMVectorGetY(vals);

	DirectX::XMFLOAT3 v;
	v.x = ( ( ( 2.0f * iMouseX ) / gWindowWidth ) - 1 ) / widthDiv;
	v.y = -( ( ( 2.0f * iMouseY ) / gWindowHeight ) - 1 ) / heightDiv;
	v.z = -1.0f;

	DirectX::XMVECTOR Det;
	DirectX::XMMATRIX m(DirectX::XMMatrixInverse(&Det, ConvertMatrix(viewTransform)));

	DirectX::XMFLOAT3 vLoad(0,0,1);
	DirectX::XMVECTOR vViewSpaceDir = DirectX::XMLoadFloat3(&v);
	vLoad = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMVECTOR vViewSpaceOrig = DirectX::XMLoadFloat3(&vLoad);

	DirectX::XMVECTOR vPickRayDir = DirectX::XMVector3TransformNormal(vViewSpaceDir,m);
	DirectX::XMVECTOR vPickRayOrig = DirectX::XMVector3TransformCoord(vViewSpaceOrig,m);

	pointOut = Vec<float>(DirectX::XMVectorGetX(vPickRayOrig),DirectX::XMVectorGetY(vPickRayOrig),DirectX::XMVectorGetZ(vPickRayOrig));
	directionOut = Vec<float>(DirectX::XMVectorGetX(vPickRayDir),DirectX::XMVectorGetY(vPickRayDir),DirectX::XMVectorGetZ(vPickRayDir));
}

float Camera::getVolUnitsPerPix() const
{
	DirectX::XMFLOAT3 distOrig_f(0, 0, 0);
	DirectX::XMVECTOR distOrig_v = DirectX::XMLoadFloat3(&distOrig_f);
	distOrig_v = DirectX::XMVector3TransformCoord(distOrig_v, ConvertMatrix(viewTransform));
	distOrig_v = DirectX::XMVector3TransformCoord(distOrig_v, ConvertMatrix(projectionTransform));
	float z = DirectX::XMVectorGetZ(distOrig_v);

	DirectX::XMFLOAT3 unitOutPt_f(2.0f/gWindowWidth, 0, z);
	DirectX::XMFLOAT3 unitOrigPt_f(0, 0, z);
	DirectX::XMVECTOR unitOutPt_v = DirectX::XMLoadFloat3(&unitOutPt_f);
	DirectX::XMVECTOR unitOrigPt_v = DirectX::XMLoadFloat3(&unitOrigPt_f);

	DirectX::XMVECTOR det;
	DirectX::XMMATRIX invProjection = DirectX::XMMatrixInverse(&det, ConvertMatrix(projectionTransform));
	
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&det, ConvertMatrix(viewTransform));

	DirectX::XMVECTOR wOutVec = DirectX::XMVector3TransformCoord(unitOutPt_v,invProjection);
	DirectX::XMVECTOR wOrigVec = DirectX::XMVector3TransformCoord(unitOrigPt_v, invProjection);
	DirectX::XMVECTOR spltW = DirectX::XMVectorSplatW(wOutVec);
	DirectX::XMVECTOR normOut = DirectX::XMVectorDivide(wOutVec, spltW);
	DirectX::XMVECTOR normOrig = DirectX::XMVectorDivide(wOrigVec, spltW);

	DirectX::XMVECTOR subVec = DirectX::XMVectorSubtract(normOut,normOrig);

	DirectX::XMVECTOR pixSizeVec = DirectX::XMVector3TransformNormal(subVec, invView);
	float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(pixSizeVec));

	return vecLength;
}

void Camera::updateViewTransform()
{
	DirectX::XMFLOAT3 eye(cameraPosition.x,cameraPosition.y,cameraPosition.z);
	DirectX::FXMVECTOR eyeVec = DirectX::XMLoadFloat3(&eye);
	DirectX::XMFLOAT3 focus(lookPosition.x,lookPosition.y,lookPosition.z);
	DirectX::FXMVECTOR focusVec = DirectX::XMLoadFloat3(&focus);
	DirectX::XMFLOAT3 up(upDirection.x,upDirection.y,upDirection.z);
	DirectX::FXMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	viewTransform = ConvertMatrix(DirectX::XMMatrixLookAtRH(eyeVec,focusVec,upVec));
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

	projectionTransform = ConvertMatrix(DirectX::XMMatrixOrthographicRH(aspectRatio*orthoHeight, orthoHeight, 0.01f, 100.0f)
		* DirectX::XMMatrixScaling(widgetScale, widgetScale, 1.0f) * DirectX::XMMatrixTranslation(ctrX, ctrY, 0.0f));
}
