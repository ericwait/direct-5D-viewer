#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"
#include "TransferObj.h"
#include "QueuePolygon.h"
#include "TextureLightingOBJ.h"
#include <vector>
#include "MexErrorMsg.h"

using std::vector;

void XloadTextureCommand(Message m){
	Image* returnedImg = (Image*)m.data;
	Vec<size_t> dims = returnedImg->getDimensions();
	unsigned char* image = returnedImg->getPixels();
	int numChannels = returnedImg->getNumChannels();
	int numFrames = returnedImg->getNumFrames();

	Vec<float> physDims = returnedImg->getPhysicalDims();

	Vec<float> scale(Vec<float>(dims) * physDims);
	scale = scale / scale.maxValue();

	GraphicObjectTypes textureType = returnedImg->getTextureType();

	if (returnedImg->getTextureType() == 1)
		textureType = GraphicObjectTypes::ProcessedVolume;

	if (gGraphicObjectNodes[GraphicObjectTypes::Border].empty())
	{
		HRESULT hr = createBorder(scale);
		if (FAILED(hr))
			mexErrMsgTxt("Could not create border!");
	}

	HRESULT hr = loadVolumeTexture(image, dims, numChannels, numFrames, scale, textureType);
	if (FAILED(hr))
		sendHrErrMessage(hr);

	setCurrentTexture(textureType);

	delete returnedImg;
}

void XresetViewCommand(Message m){
	gRenderer->resetRootWorldTransform();
	gCameraDefaultMesh->resetCamera();
}

void XtransferUpdateCommand(Message m){
	TransferObj* sentTranfser = (TransferObj*)m.data;

	GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;
	char buff[96];

	strncpy_s(buff, sentTranfser->buff, strlen(sentTranfser->buff));

	if (_strcmpi("original", buff) == 0)
		textureType = GraphicObjectTypes::OriginalVolume;
	else if (_strcmpi("processed", buff) == 0)
		textureType = GraphicObjectTypes::ProcessedVolume;

	int fvtIdx = textureType - GraphicObjectTypes::OriginalVolume;

	size_t numElem = sentTranfser->numElem;

	firstVolumeTextures[fvtIdx]->setTransferFunction(sentTranfser->chan, sentTranfser->transferFunction);
	firstVolumeTextures[fvtIdx]->setRange(sentTranfser->chan, sentTranfser->ranges);
	firstVolumeTextures[fvtIdx]->setColor(sentTranfser->chan, sentTranfser->color, sentTranfser->alphaMod);

	delete sentTranfser;
}

void XaddPolygonCommand(Message m){
	if (gRenderer == NULL) return;

	/*if (!gGraphicObjectNodes[GraphicObjectTypes::CellPolygons].empty())
	{
		//gRenderer->getMutex();
		for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellPolygons].size(); ++j)
		{
			gGraphicObjectNodes[GraphicObjectTypes::CellPolygons][j]->releaseRenderResources();
			delete gGraphicObjectNodes[GraphicObjectTypes::CellPolygons][j];
		}

		gGraphicObjectNodes[GraphicObjectTypes::CellPolygons].clear();

		gRenderer->updateRenderList();
		//gRenderer->releaseMutex();
	}

	if (hullRootNodes.size() != gRenderer->getNumberOfFrames())
	{
		for (int i = 0; i < hullRootNodes.size(); ++i)
			delete hullRootNodes[i];
	}
	*/
	if (hullRootNodes.empty()){
		hullRootNodes.resize(gRenderer->getNumberOfFrames());
		for (unsigned int i = 0; i < gRenderer->getNumberOfFrames(); ++i)
			hullRootNodes[i] = NULL;
	}

	QueuePolygon* polygon = (QueuePolygon*)m.data;

	if (hullRootNodes[polygon->getFrame()] == NULL){
		hullRootNodes[polygon->getFrame()] = new SceneNode();
		gRenderer->attachToRootScene(hullRootNodes[polygon->getFrame()], Renderer::Section::Main, polygon->getFrame());
	}

	GraphicObjectNode* oldNode = getGlobalGraphicsObject(GraphicObjectTypes::Polygons, polygon->getIndex());
	if (oldNode)
	{
		sendErrMessage("You can't add a hull that already exists!");
		return;
	}

	PolygonObject* curPolygonObj = createPolygonObject(polygon->getfaceData(), polygon->getNumFaces(), polygon->getvertData(), polygon->getNumVerts(), polygon->getnormData(), polygon->getNumNormals(), gCameraDefaultMesh);
	curPolygonObj->setColor(Vec<float>((float)polygon->getcolorData()[0], (float)polygon->getcolorData()[1], (float)polygon->getcolorData()[2]), 1.0f);
	curPolygonObj->setIndex(polygon->getIndex());
	curPolygonObj->setLabel(polygon->getLabel());
	GraphicObjectNode* curPolygonNode = new GraphicObjectNode(curPolygonObj);
	curPolygonNode->setWireframe(true);
	curPolygonNode->attachToParentNode(hullRootNodes[polygon->getFrame()]);

	insertGlobalGraphicsObject(GraphicObjectTypes::Polygons, curPolygonNode);

	delete polygon;
}

void XaddPolygonsCommand(Message m){
	vector<QueuePolygon*>* polygons = (vector<QueuePolygon*>*)m.data;

	for (vector<QueuePolygon*>::iterator polygon = polygons->begin(); polygon != polygons->end(); ++polygon) {
		if (gRenderer == NULL) return;

		int frame = (*polygon)->getFrame();

		if (hullRootNodes.empty()){
			hullRootNodes.resize(gRenderer->getNumberOfFrames());
			for (unsigned int i = 0; i < gRenderer->getNumberOfFrames(); ++i)
				hullRootNodes[i] = NULL;
		}

		if (hullRootNodes[frame] == NULL){
			hullRootNodes[frame] = new SceneNode();
			gRenderer->attachToRootScene(hullRootNodes[frame], Renderer::Section::Main, frame);
		}

		GraphicObjectNode* oldNode = getGlobalGraphicsObject(GraphicObjectTypes::Polygons, (*polygon)->getIndex());
		if (oldNode)
		{
			sendErrMessage("You can't add a hull that already exists!");
			return;
		}

		PolygonObject* curPolygonObj = createPolygonObject((*polygon)->getfaceData(), (*polygon)->getNumFaces(), (*polygon)->getvertData(), (*polygon)->getNumVerts(), (*polygon)->getnormData(), (*polygon)->getNumNormals(), gCameraDefaultMesh);
		curPolygonObj->setColor(Vec<float>((float)(*polygon)->getcolorData()[0], (float)(*polygon)->getcolorData()[1], (float)(*polygon)->getcolorData()[2]), 1.0f);
		curPolygonObj->setIndex((*polygon)->getIndex());
		curPolygonObj->setLabel((*polygon)->getLabel());
		GraphicObjectNode* curPolygonNode = new GraphicObjectNode(curPolygonObj);
		curPolygonNode->setWireframe(true);
		curPolygonNode->attachToParentNode(hullRootNodes[(*polygon)->getFrame()]);

		insertGlobalGraphicsObject(GraphicObjectTypes::Polygons, curPolygonNode);

		delete *polygon;
	}
	delete polygons;
}

void XremovePolygonCommand(Message m){
	int* labelPtr = (int*)m.data;
	int inputLabel = *labelPtr;

	GraphicObjectNode* removeNode = getGlobalGraphicsObject(GraphicObjectTypes::Polygons,inputLabel);
	if (!removeNode)
		return;

	removeGlobalGraphicsObject(GraphicObjectTypes::Polygons, inputLabel);
	removeNode->detatchFromParentNode();

	delete removeNode;

	gRenderer->updateRenderList();

	delete labelPtr;
}

void XpeelUpdateCommand(Message m){
	float* x = (float*)m.data;
	gRenderer->setClipChunkPercent(*x);

	delete x;
}

void XplayCommand(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	gPlay = on;

	delete doublePtr;
}

void XrotateCommand(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	gRotate = on;

	delete doublePtr;
}

void XshowLabelsCommand(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	gRenderer->setLabels(on);

	delete doublePtr;
}


void XtextureLightingUpdateCommand(Message m){
	TextureLightingObj* localLightObj = (TextureLightingObj*)m.data;
	firstVolumeTextures[localLightObj->index]->setLightOn(localLightObj->value);

	delete localLightObj;
}

void XtextureAttenUpdateCommand(Message m){
	TextureLightingObj* localLightObj = (TextureLightingObj*)m.data;
	firstVolumeTextures[localLightObj->index]->setAttenuationOn(localLightObj->value);

	delete localLightObj;
}

void XsegmentationLighting(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	toggleSegmentaionLighting(on);
	delete doublePtr;
}

void XcaptureSpinMovieCommand(Message m){
	gRenderer->resetRootWorldTransform();
	//				gCameraDefaultMesh->resetCamera();

	gCapture = true;
	gRotate = true;
}

void XviewTextureCommand(Message m){

	GraphicObjectTypes* textureType = (GraphicObjectTypes*)m.data;

	setCurrentTexture(*textureType);

	delete[] m.data;
}

void XviewSegmentationCommand(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	toggleSegmentationResults(on);

	delete doublePtr;
}

void XwireframeSegmentationCommand(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	toggleSegmentaionWireframe(on);

	delete doublePtr;
}

void XsetFrameCommand(Message m){
	int* doublePtr = (int*)m.data;
	int curFrame = *doublePtr;

	gRenderer->setCurrentFrame(curFrame);

	delete doublePtr;
}

void XsetViewOriginCommand(Message m){
	double* doublePtr = (double*)m.data;

	gRenderer->convertToScreenSpace(doublePtr,1);

	gRenderer->setWorldOrigin(Vec<float>((float)(doublePtr[0]), (float)(doublePtr[1]), (float)(doublePtr[2])));

	delete[] doublePtr;
}

void XdisplayPolygonsCommand(Message m){
	std::set<int>* hullset = (std::set<int>*)m.data;

	toggleSelectedCell(*hullset);
	delete hullset;
}

void XdeleteAllPolygonsCommand(Message m){
	const GraphicObjectTypes objType = GraphicObjectTypes::Polygons;

	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[objType].begin();
	for ( ; objectIter != gGraphicObjectNodes[objType].end(); ++objectIter )
	{
		GraphicObjectNode* node = objectIter->second;
		node->detatchFromParentNode();
		delete node;
	}

	gGraphicObjectNodes[objType].clear();
	gRenderer->updateRenderList();
}

void XcaptureWindow()
{
	gRenderer->renderAll();

	gRenderer->captureWindow(NULL);
}
