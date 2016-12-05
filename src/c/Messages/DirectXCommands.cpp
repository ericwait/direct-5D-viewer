#include "Global/Globals.h"
#include "Global/Vec.h"
#include "mex.h"
#include "Mex/MexFunctions.h"
#include "Image.h"
#include "TransferObj.h"
#include "QueuePolygon.h"
#include "D3d/TextureLightingObj.h"
#include <vector>
#include "MexErrorMsg.h"
#include "D3d/MessageProcessor.h"

using std::vector;

void XloadTextureCommand(Message m){
	Image* returnedImg = (Image*)m.data;
	Vec<size_t> dims = returnedImg->getDimensions();
	unsigned char* image = returnedImg->getPixels();
	int numChannels = returnedImg->getNumChannels();
	int numFrames = returnedImg->getNumFrames();

	Vec<float> physDims = returnedImg->getPhysicalDims();

	Vec<float> scale(Vec<float>(dims) * physDims);
	scale = scale;

	GraphicObjectTypes textureType = returnedImg->getTextureType();

	if (returnedImg->getTextureType() == 1)
		textureType = GraphicObjectTypes::ProcessedVolume;

	if (gGraphicObjectNodes[GraphicObjectTypes::Border].empty())
	{
		HRESULT hr = createBorder(scale/scale.maxValue());
		if (FAILED(hr))
			mexErrMsgTxt("Could not create border!");
	}

	HRESULT hr = loadVolumeTexture(image, dims, numChannels, numFrames, scale, textureType);
	if (FAILED(hr))
		sendHrErrMessage(hr);

	setCurrentTexture(textureType);

	delete returnedImg;
	gMsgQueueToMex.addMessage("loadDone", 0);
}

void XresetViewCommand(Message m){
	gRenderer->resetRootWorldTransform();
	gCameraDefaultMesh->resetCamera();
}

void XtransferUpdateCommand(Message m){
	TransferObj* sentTranfser = (TransferObj*)m.data;

	GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;
	char buff[256];

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

void XaddPolygonsCommand(Message m){
	vector<QueuePolygon*>* polygons = (vector<QueuePolygon*>*)m.data;

	for (vector<QueuePolygon*>::iterator polygon = polygons->begin(); polygon != polygons->end(); ++polygon) {
		if (gRenderer == NULL) return;

		QueuePolygon* curPoly = *polygon;

		int frame = curPoly->getFrame();

		if(frame<0)
		{
			char buff[128];
			sprintf(buff, "You can't add a polygon to a negative frame! Polygon:%d, Frame:%d", curPoly->getIndex(),frame);
			sendErrMessage(buff);
			return;
		}

		if (hullRootNodes.empty()){
			hullRootNodes.resize(gRenderer->getNumberOfFrames());
			for (unsigned int i = 0; i < gRenderer->getNumberOfFrames(); ++i)
				hullRootNodes[i] = NULL;
		}

		if (hullRootNodes[frame] == NULL){
			hullRootNodes[frame] = new SceneNode();
			gRenderer->attachToRootScene(hullRootNodes[frame], Renderer::Section::Main, frame);
		}

		GraphicObjectNode* oldNode = getGlobalGraphicsObject(GraphicObjectTypes::Polygons, curPoly->getIndex());
		if (oldNode)
		{
			char buff[128];
			sprintf(buff,"You can't add a polygon that already exists! Polygon:%d", curPoly->getIndex());
			sendErrMessage(buff);
			return;
		}

		PolygonObject* curPolygonObj = createPolygonObject(curPoly->getfaceData(), curPoly->getNumFaces(), curPoly->getvertData(), curPoly->getNumVerts(), curPoly->getnormData(), curPoly->getNumNormals(), gCameraDefaultMesh);
		double* color = curPoly->getcolorData();
		curPolygonObj->setColor(Vec<float>((float)(color[0]), (float)(color[1]), (float)(color[2])), (float)(color[3]));
		curPolygonObj->setIndex(curPoly->getIndex());
		curPolygonObj->setLabel(curPoly->getLabel());
		GraphicObjectNode* curPolygonNode = new GraphicObjectNode(curPolygonObj);
		curPolygonNode->setWireframe(true);
		curPolygonNode->attachToParentNode(hullRootNodes[curPoly->getFrame()]);

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

void XtextureAttenUpdateCommand(Message m)
{
	bool* on = (bool*)m.data;

	for(int i = 0; i<firstVolumeTextures.size(); ++i)
	{
		if(NULL!=firstVolumeTextures[i])
		{
			firstVolumeTextures[i]->setAttenuationOn(*on);
		}
	}

	delete on;
}

void XpolygonLighting(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	togglePolygonLighting(on);
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

void XtoggleWireframe(Message m){
	double* doublePtr = (double*)m.data;
	double onD = *doublePtr;
	bool on = onD > 0;

	togglePolygonWireframe(on);

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

	gRenderer->convertToWorldSpace(doublePtr,1);

	gRenderer->setWorldOrigin(Vec<float>((float)(doublePtr[0]), (float)(doublePtr[1]), (float)(doublePtr[2])));

	delete[] doublePtr;
}

void XdisplayPolygonsCommand(Message m){
	std::set<int>* hullset = (std::set<int>*)m.data;

	toggleSelectedPolygon(*hullset);
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

void XcaptureWindow(void* imageOut/*=NULL*/)
{
	gRenderer->renderAll();

	if(imageOut!=NULL)
	{
		DWORD dwBmpSize = 0;
		BITMAPINFOHEADER bi;
		unsigned char* lpbitmap = gRenderer->captureWindow(dwBmpSize, bi);
		Image* im = (Image*)imageOut;
		im->setPixels(lpbitmap);
		im->setDimensions(Vec<size_t>(bi.biWidth, bi.biHeight, 3));

		gMsgQueueToMex.addMessage("loadDone",0);
	}
	else
		gRenderer->captureWindow(NULL);
}

void XsetBackgroundColor(Message m)
{
	Vec<float>* bc = (Vec<float>*)m.data;
	gRenderer->setBackgroundColor(*bc);
	delete bc;
}

void XsetWindowSize(Message m)
{
	double* dims = (double*)m.data;
	int width = round(dims[0]);
	int height = round(dims[1]);

	ClientResize(gWindowHandle, width, height);
}