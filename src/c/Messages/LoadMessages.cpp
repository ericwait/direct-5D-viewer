#include "LoadMessages.h"
#include "LoadData.h"
#include "MessageHelpers.h"

#include "D3d/VolumeInfo.h"

#include "Global/Globals.h"
#include "Global/ErrorMsg.h"


MessageInitVolume::MessageInitVolume(int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physSize, bool columnMajor)
	: numFrames(numFrames), numChannels(numChannels), dims(dims), physicalSize(physSize), columnMajor(true)
{}

bool MessageInitVolume::process()
{
	if ( !gRenderer )
		return false;

	HRESULT hr = initVolume(numFrames, numChannels, dims, physicalSize, columnMajor);
	if ( FAILED(hr) )
	{
		sendHrErrMessage(hr);
		return false;
	}

	return true;
}



MessageLoadTextureFrame::MessageLoadTextureFrame(GraphicObjectTypes type, int frame, unsigned char* data)
	: textureType(type), frame(frame), imageData(data)
{}

bool MessageLoadTextureFrame::process()
{
	if ( !gRenderer )
		false;

	const VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( !info )
	{
		sendErrMessage("Must initialize volume first with InitVolume!");
		return false;
	}

	HRESULT hr = loadTextureFrame(textureType, frame, imageData);
	if ( FAILED(hr) )
	{
		sendHrErrMessage(hr);
		return false;
	}

	//TODO: Should we also set visibility here?

	return true;
}



MessageClearTextureFrame::MessageClearTextureFrame(GraphicObjectTypes type, int frame)
	: textureType(type), frame(frame)
{}

bool MessageClearTextureFrame::process()
{
	if(!gRenderer)
		return false;

	clearTextureFrame(frame, textureType);
	return true;
}



MessageClearAllTexture::MessageClearAllTexture(GraphicObjectTypes inType)
	: textureType(inType)
{}


bool MessageClearAllTexture::process()
{
	if(!gRenderer)
		return false;

	clearAllTextures(textureType);
	return true;
}




MessageLoadTexture::MessageLoadTexture(GraphicObjectTypes inType, unsigned char* inData)
	: textureType(inType), imageData(inData)
{}

bool MessageLoadTexture::process()
{
	if ( !gRenderer )
		return false;

	const VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( !info )
	{
		sendErrMessage("Must initialize volume first with InitVolume!");
		return false;
	}

	HRESULT hr = loadVolumeTexture(imageData, textureType);
	if ( FAILED(hr) )
	{
		sendHrErrMessage(hr);
		return false;
	}

	// Reset visibility for all volume types (assumes they are at the end of the list
	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::NumGO; ++i )
		setObjectTypeVisibility((GraphicObjectTypes)i, (i == textureType));

	return true;
}


std::vector<SceneNode*> MessageLoadPolys::rootNodes;

MessageLoadPolys::MessageLoadPolys(size_t numPolys)
{
	polygons.reserve(numPolys);
}

MessageLoadPolys::~MessageLoadPolys()
{
	for ( int i=0; i < polygons.size(); ++i )
		delete polygons[i];

	polygons.clear();
}

void MessageLoadPolys::addPoly(QueuePolygon* newPolygon)
{
	polygons.push_back(newPolygon);
}

bool MessageLoadPolys::process()
{
	if ( !gRenderer )
		return false;

	VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( !info )
		return false;

	for ( int i=0; i < polygons.size(); ++i )
	{
		QueuePolygon* poly = polygons[i];

		int index = poly->getIndex();
		int frame = poly->getFrame();

		if ( frame < 0 )
		{
			char buff[128];
			sprintf(buff, "Cannot add a polygon to a negative frame! Polygon:%d, Frame:%d", index, frame);
			sendErrMessage(buff);

			return false;
		}

		GraphicObjectNode* oldNode = gRenderer->findSceneObject(GraphicObjectTypes::Polygons, index);
		if ( oldNode )
		{
			char buff[128];
			sprintf(buff, "Polygon index already exists! Polygon:%d", index);
			sendErrMessage(buff);

			return false;
		}

		// TODO: Get rid of the hullRoot globals
		if ( rootNodes.empty() )
		{
			rootNodes.resize(gRenderer->getNumberOfFrames());
			for ( int i = 0; i < gRenderer->getNumberOfFrames(); ++i )
				rootNodes[i] = NULL;
		}

		if ( frame >= gRenderer->getNumberOfFrames() )
		{
			char buff[128];
			sprintf(buff, "Polygon frame: %d is greater than movie frames: %d!", frame, gRenderer->getNumberOfFrames());
			sendErrMessage(buff);

			return false;
		}

		SceneNode* frameNode = rootNodes[frame];
		if ( frameNode == NULL )
		{
			frameNode = new SceneNode(GraphicObjectTypes::Group);
			gRenderer->attachToRootScene(frameNode, Renderer::Section::Main, frame);
			rootNodes[frame] = frameNode;
		}

		double* color = poly->getcolorData();
		Color polyColor(color[0], color[1], color[2], color[3]);

		// TODO: Can we build this into the local to parent without screwing up normals?
		info->imageToModelSpace(poly->getvertData(), poly->getNumVerts());

		std::shared_ptr<MeshPrimitive> polyMesh = createPolygonMesh(poly->getfaceData(), poly->getNumFaces(), poly->getvertData(), poly->getNumVerts(), poly->getnormData(), poly->getNumNormals(), polyColor);
		std::shared_ptr<PolygonMaterial> polyMat = std::make_shared<PolygonMaterial>(gRenderer);

		GraphicObjectNode* polyNode = new GraphicObjectNode(index, GraphicObjectTypes::Polygons, polyMesh, polyMat);
		polyNode->setLabel(poly->getLabel());
		polyNode->setWireframe(true);

		polyNode->attachToParentNode(frameNode);
	}

	return true;
}



bool MessageDeletePoly::process()
{
	if ( !gRenderer )
		return true;

	GraphicObjectNode* removeNode = gRenderer->findSceneObject(GraphicObjectTypes::Polygons, index);
	if ( !removeNode )
		return true;

	removeNode->detatchFromParentNode();
	delete removeNode;

	return true;
}



bool MessageDeleteAllPolys::process()
{
	if ( !gRenderer )
		return true;

	gRenderer->removeSceneObjects(GraphicObjectTypes::Polygons);

	return true;
}
