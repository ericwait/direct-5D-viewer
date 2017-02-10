#include "LoadMessages.h"
#include "LoadData.h"
#include "MessageHelpers.h"

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
	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::VTend; ++i )
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

	for ( int i=0; i < polygons.size(); ++i )
	{
		QueuePolygon* poly = polygons[i];

		if ( poly->getFrame() < 0 )
		{
			char buff[128];
			sprintf(buff, "Cannot add a polygon to a negative frame! Polygon:%d, Frame:%d", poly->getIndex(), poly->getFrame());
			sendErrMessage(buff);

			return false;
		}

		GraphicObjectNode* oldNode = getGlobalGraphicsObject(GraphicObjectTypes::Polygons, poly->getIndex());
		if ( oldNode )
		{
			char buff[128];
			sprintf(buff, "Polygon index already exists! Polygon:%d", poly->getIndex());
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

		if ( rootNodes[poly->getFrame()] == NULL )
		{
			rootNodes[poly->getFrame()] = new SceneNode(GraphicObjectTypes::Group);
			gRenderer->attachToRootScene(rootNodes[poly->getFrame()], Renderer::Section::Main, poly->getFrame());
		}

		double* color = poly->getcolorData();
		std::shared_ptr<MeshPrimitive> polyMesh = createPolygonMesh(poly->getfaceData(), poly->getNumFaces(), poly->getvertData(), poly->getNumVerts(), poly->getnormData(), poly->getNumNormals());
		std::shared_ptr<SingleColoredMaterial> polyMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>((float)(color[0]), (float)(color[1]), (float)(color[2])), (float)(color[3]));

		GraphicObjectNode* polyNode = new GraphicObjectNode(poly->getIndex(), GraphicObjectTypes::Polygons, polyMesh, polyMat);
		polyNode->setLabel(poly->getLabel());
		polyNode->setWireframe(true);

		polyNode->attachToParentNode(rootNodes[poly->getFrame()]);
		insertGlobalGraphicsObject(GraphicObjectTypes::Polygons, polyNode);
	}

	return true;
}



bool MessageDeletePoly::process()
{
	GraphicObjectNode* removeNode = getGlobalGraphicsObject(GraphicObjectTypes::Polygons, index);
	// TODO: Should this be a processing error?
	if ( !removeNode )
		return true;

	removeGlobalGraphicsObject(GraphicObjectTypes::Polygons, index);
	removeNode->detatchFromParentNode();

	delete removeNode;

	// TODO: Remove renderlist this entirely?
	gRenderer->updateRenderList();

	return true;
}



bool MessageDeleteAllPolys::process()
{
	const GraphicObjectTypes polyType = GraphicObjectTypes::Polygons;

	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[polyType].begin();
	for ( ; objectIter != gGraphicObjectNodes[polyType].end(); ++objectIter )
	{
		GraphicObjectNode* node = objectIter->second;
		node->detatchFromParentNode();

		delete node;
	}

	gGraphicObjectNodes[polyType].clear();

	gRenderer->updateRenderList();

	return true;
}
