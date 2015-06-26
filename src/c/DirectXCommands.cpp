#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"
#include "TransferObj.h"
#include "QueuePolygon.h"
#include "TextureLightingOBJ.h"

void XloadTextureCommand(Message m){
	Image* returnedImg = (Image*)m.data;
	Vec<size_t> dims = returnedImg->getDimensions();
	unsigned char* image = returnedImg->getPixels();
	int numChannels = returnedImg->getNumChannels();
	int numFrames = returnedImg->getNumFrames();

	Vec<float> physDims = returnedImg->getPhysicalDims();

	Vec<float> scale(dims);
	scale = scale / scale.maxValue();

	scale.x *= physDims.x/physDims.minValue();
	scale.y *= physDims.y/physDims.minValue();
	scale.z *= physDims.z/physDims.minValue();

	GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;

	if (returnedImg->getNumArgs() == 1)
		textureType = GraphicObjectTypes::ProcessedVolume;

	if (gGraphicObjectNodes[GraphicObjectTypes::Border].empty())
	{
		HRESULT hr = createBorder(scale);
		if (FAILED(hr))
			mexErrMsgTxt("Could not create border!");
	}

	HRESULT hr = loadVolumeTexture(image, dims, numChannels, numFrames, scale, textureType);
	if (FAILED(hr))
		mexErrMsgTxt("Could not load texture!");

	setCurrentTexture(textureType);
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
}

void XaddHullCommand(Message m){
	if (gRenderer == NULL) return;

	/*if (!gGraphicObjectNodes[GraphicObjectTypes::CellHulls].empty())
	{
		//gRenderer->getMutex();
		for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->releaseRenderResources();
			delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j];
		}

		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].clear();

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

	if (hullRootNodes[polygon->frame] == NULL){
		hullRootNodes[polygon->frame] = new SceneNode();
		gRenderer->attachToRootScene(hullRootNodes[polygon->frame], Renderer::Section::Main, polygon->frame);
	}

	int hullIdx = -1;
	for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
	{
		int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
		if (label == polygon->label)
		{
			// Throw Matlab error if we try to add a hull that exists
			gMexMessageQueueOut.addErrorMessage("You can't add a hull that already exists!");
			hullIdx = j;
			break;
		}
	}

	CellHullObject* curHullObj = createCellHullObject(polygon->getfaceData(), polygon->numFaces, polygon->getvertData(), polygon->numVerts, polygon->getnormData(), polygon->numNormals, gCameraDefaultMesh);
	curHullObj->setColor(Vec<float>((float)polygon->getcolorData()[0], (float)polygon->getcolorData()[1], (float)polygon->getcolorData()[2]), 1.0f);
	curHullObj->setLabel(polygon->label);
	curHullObj->setTrack(polygon->track);
	GraphicObjectNode* curHullNode = new GraphicObjectNode(curHullObj);
	curHullNode->setWireframe(true);
	curHullNode->attachToParentNode(hullRootNodes[polygon->frame]);
	gGraphicObjectNodes[GraphicObjectTypes::CellHulls].push_back(curHullNode);
}

void XremoveHullCommand(Message m){
	int* labelPtr = (int*)m.data;
	int inputLabel = *labelPtr;

	int hullIdx = -1;
	for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
	{
		int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
		if (label == inputLabel)
		{
			hullIdx = j;
			break;
		}
	}

	if (hullIdx > -1){
		delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx];
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].erase(gGraphicObjectNodes[GraphicObjectTypes::CellHulls].begin() + hullIdx);
		gRenderer->updateRenderList();
	}
}

void XpeelUpdateCommand(Message m){
	float* x = (float*)m.data;
	gRenderer->setClipChunkPercent(*x);
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
}


void XtextureLightingUpdateCommand(Message m){
	TextureLightingObj* localLightObj = (TextureLightingObj*)m.data;
	firstVolumeTextures[localLightObj->index]->setLightOn(localLightObj->value);
}

void XtextureAttenUpdateCommand(Message m){
	TextureLightingObj* localLightObj = (TextureLightingObj*)m.data;
	firstVolumeTextures[localLightObj->index]->setAttenuationOn(localLightObj->value);
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

	GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;

	if (_strcmpi("original", (char*)m.data) == 0)
		textureType = GraphicObjectTypes::OriginalVolume;
	else if (_strcmpi("processed", (char*)m.data) == 0)
		textureType = GraphicObjectTypes::ProcessedVolume;

	setCurrentTexture(textureType);

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

	gRenderer->setWorldOrigin(Vec<float>((float)(doublePtr[0]), (float)(doublePtr[1]), (float)(doublePtr[2])));

	delete[] doublePtr;
}

void XdisplayHullsCommand(Message m){
	std::set<int>* hullset = (std::set<int>*)m.data;

	toggleSelectedCell(*hullset);
	delete hullset;
}