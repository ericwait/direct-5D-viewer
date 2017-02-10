#include "ViewMessages.h"
#include "MessageHelpers.h"

#include "Global/Globals.h"
#include "D3d/MessageProcessor.h"
#include "D3d/VolumeInfo.h"

#include "D3d/EigenHelpers.h"

#include <cmath>


bool MessageSetWindowSize::process()
{
	ClientResize(gWindowHandle, width, height);

	return true;
}



bool MessageResetView::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->resetRootWorldTransform();
	gCameraDefaultMesh->resetCamera();

	return true;
}



bool MessageSetViewOrigin::process()
{
	if ( !gRenderer )
		return false;

	VolumeInfo* info = gRenderer->getVolumeInfo();

	info->imageToModelSpace(origin.e, 1);
	gRenderer->setWorldOrigin(origin);

	return true;
}



bool MessageSetViewRotation::process()
{
	if ( !gRenderer )
		return false;

	double radians = angle / 180.0 * M_PI;

	Eigen::Affine3f applyRot = RotationTransform(radians, Eigen::Vector3f(axis.e[0], axis.e[1], axis.e[2]));

	DirectX::XMMATRIX dxMat = ConvertMatrix(applyRot * ConvertMatrix(gRenderer->getRootWorldRotation()));
	gRenderer->setWorldRotation(dxMat);

	return true;
}



bool MessageSetWorldRotation::process()
{
	if ( !gRenderer )
		return false;

	double radians = angle / 180.0 * M_PI;

	Eigen::Affine3f applyRot = RotationTransform(radians, Eigen::Vector3f(axis.e[0], axis.e[1], axis.e[2]));

	DirectX::XMMATRIX dxMat = ConvertMatrix((ConvertMatrix(gRenderer->getRootWorldRotation()) * applyRot).matrix());
	gRenderer->setWorldRotation(dxMat);

	return true;
}



bool MessageSetFrontClipPlane::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->FrontClipPos(clipDistance);

	return true;
}



bool MessageShowFrame::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->setFrameNumOn(visible);

	return true;
}



bool MessageShowLabels::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->setLabels(visible);

	return true;
}



bool MessageShowScale::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->setscaleBarOn(visible);

	return true;
}



bool MessageSetBackgroundColor::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->setBackgroundColor(color);
	return true;
}



bool MessageSetObjectColor::process()
{
	if ( !gRenderer )
		return false;

	setObjectTypeColor(type, color, 1.0f);
	return true;
}



bool MessageMoveCamera::process()
{
	if ( !gRenderer )
		return false;

	gCameraDefaultMesh->move(delta);

	return true;
}



void MessageShowPolys::setPoly(int index)
{
	indices.insert(index);
}

bool MessageShowPolys::process()
{
	if ( !gRenderer )
		return false;

	// Show/hide all polygons if the list is empty
	if ( indices.empty() )
	{
		setObjectTypeVisibility(GraphicObjectTypes::Polygons, visible);
	}
	else
	{
		for ( auto it = indices.begin(); it != indices.end(); ++it )
		{
			GraphicObjectNode* node = getGlobalGraphicsObject(GraphicObjectTypes::Polygons, *it);
			node->setRenderable(visible);
		}
	}

	return true;
}



bool MessageShowObjectType::process()
{
	if ( !gRenderer )
		return false;

	setObjectTypeVisibility(type, visible);

	return true;
}



bool MessageSetPolyWireframe::process()
{
	if ( !gRenderer )
		return false;

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	for ( auto itr : gGraphicObjectNodes[cellType] )
		itr.second->setWireframe(wireframe);

	return true;
}



bool MessageSetPolyLighting::process()
{
	if ( !gRenderer )
		return false;

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	for ( auto itr : gGraphicObjectNodes[cellType] )
		itr.second->setLightOn(lightingOn);

	return true;
}



void MessageUpdateTransferFcn::setTransferFcn(Vec<float> transferFcnIn)
{
	transferFcn = transferFcnIn;
}

void MessageUpdateTransferFcn::setRange(Vec<float> rangeIn)
{
	range = rangeIn;
}

void MessageUpdateTransferFcn::setColor(Vec<float> colorIn, float alphaIn)
{
	color = colorIn;
	alpha = alphaIn;
}

bool MessageUpdateTransferFcn::process()
{
	if ( !gRenderer )
		return false;

	VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( !info )
		return false;

	std::shared_ptr<VolumeParams> sharedParams = info->getParams(type);
	if ( !sharedParams )
		return false;

	sharedParams->setTransferFunction(channel, transferFcn);
	sharedParams->setRange(channel, range);
	sharedParams->setColor(channel, color, alpha);

	return true;
}



bool MessageSetTextureLighting::process()
{
	if ( !gRenderer )
		return false;

	VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( !info )
		return false;

	std::shared_ptr<VolumeParams> sharedParams = info->getParams(type);
	if ( !sharedParams )
		return false;

	sharedParams->setLightOn(lightingOn);

	return true;
}



bool MessageSetTextureAttenuation::process()
{
	if ( !gRenderer )
		return false;

	VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( !info )
		return false;

	std::shared_ptr<VolumeParams> sharedParams = info->getParams(type);
	if ( !sharedParams )
		return false;

	sharedParams->setAttenuationOn(attenuate);

	return true;
}
