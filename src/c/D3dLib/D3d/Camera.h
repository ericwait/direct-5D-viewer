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

#pragma once
#include "Vec.h"
#include "Renderer.h"

#include <DirectXMath.h>
#include <Eigen/Dense>
#include "EigenToFromDirectX.h"

class Camera
{
public:
	Camera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn, Vec<int> viewportSizeIn);
	virtual ~Camera() {}

	void move(Vec<float> delta);
	float getNearZ() const { return nearZ; }
	void setNearZ(float val);
	void resetCamera();
	void setCameraPosition(Vec<float> cameraPositionIn);
	void setLookPosition(Vec<float> lookPositionIn);
	void setUpDirection(Vec<float> upDirectionIn);
	void setCamera(Vec<float> cameraPositionIn, Vec<float> lookPositionIn, Vec<float> upDirectionIn);
	virtual void updateProjectionTransform();
	virtual void setViewportSize(Vec<int> viewportSizeIn);

	DirectX::XMMATRIX getProjectionTransform() const {return ConvertMatrix(projectionTransform);}
	DirectX::XMMATRIX getViewTransform() const {return ConvertMatrix(viewTransform);}
	void getRay(int iMouseX, int iMouseY, Vec<float>& pointOut, Vec<float>& directionOut);
	float getVolUnitsPerPix() const;

protected:
	Camera(){}

	static const float fovY;
	static const float farPlane;
	static const float defNearPlane;

	virtual void updateViewTransform();
	Vec<float> cameraPosition = Vec<float>(0.0);
	Vec<float> lookPosition   = Vec<float>(0.0);
	Vec<float> upDirection    = Vec<float>(0.0);

	Vec<float> defaultCameraPosition = Vec<float>(0.0);
	Vec<float> defaultLookPosition   = Vec<float>(0.0);
	Vec<float> defaultUpDirection    = Vec<float>(0.0);

	float nearZ;
	Vec<int> viewportSize = Vec<int>(0);

	Eigen::Matrix4f viewTransform;
	Eigen::Matrix4f projectionTransform;
};


class OrthoCamera : public Camera
{
public:
	OrthoCamera(Vec<float> cameraPostionIn, Vec<float> lookPostionIn, Vec<float> upDirectionIn, Vec<int> viewportSizeIn);

	void updateProjectionTransform();

private:
	OrthoCamera(){}
};

class TextCamera : public Camera
{
public:
	TextCamera(Vec<float> cameraPos, Vec<float> lookPos, Vec<float> upVec, Vec<int> vpSize);

	virtual void updateProjectionTransform();

private:
	TextCamera(){}

	virtual void updateViewTransform();
};
