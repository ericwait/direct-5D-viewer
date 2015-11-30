#include "Vec.h"
#include "mex.h"
#include "MessageQueue.h"
#include "windows.h"
#include "Globals.h"
#include "MexFunctions.h"
#include "QueuePolygon.h"
#include <set>
#include <vector>

using std::vector;

std::vector<VolumeTextureObject*> firstVolumeTextures;
std::vector<SceneNode*> hullRootNodes;

void setCurrentTexture(GraphicObjectTypes textureType)
{
	if (gRenderer == NULL) return;

	int fvtIdx = textureType - GraphicObjectTypes::OriginalVolume;

	//gRenderer->getMutex();

	for (int i = 0; i < firstVolumeTextures.size(); ++i)
	{
		int idx = GraphicObjectTypes::OriginalVolume + i;
		if (idx == GraphicObjectTypes::VTend)
			break;

		bool render = (i == fvtIdx);
		std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[idx].begin();
		for ( ; objectIter != gGraphicObjectNodes[idx].end(); ++objectIter )
			objectIter->second->setRenderable(render);
	}

	//gRenderer->releaseMutex();
}

void toggleSegmentationResults(bool on)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[cellType].begin();
	for ( ; objectIter != gGraphicObjectNodes[cellType].end(); ++objectIter )
		objectIter->second->setRenderable(on);

	//gRenderer->releaseMutex();
}

void toggleSegmentaionWireframe(bool wireframe)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[cellType].begin();
	for ( ; objectIter != gGraphicObjectNodes[cellType].end(); ++objectIter )
		objectIter->second->setWireframe(wireframe);

	//gRenderer->releaseMutex();
}

void toggleSegmentaionLighting(bool lighting)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[cellType].begin();
	for ( ; objectIter != gGraphicObjectNodes[cellType].end(); ++objectIter )
		objectIter->second->setLightOn(lighting);

	//gRenderer->releaseMutex();
}

void toggleSelectedCell(std::set<int> labels)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[cellType].begin();
	for ( ; objectIter != gGraphicObjectNodes[cellType].end(); ++objectIter )
	{
		GraphicObjectNode* node = objectIter->second;
		if ( labels.count(node->getPolygonLabel()) > 0 )
			node->setRenderable(true);
		else
			node->setRenderable(false);
	}

	//gRenderer->releaseMutex();
}

HRESULT updatePolygons(const mxArray* hulls)
{
	if (gRenderer == NULL) return E_FAIL;

	size_t numPolygons = mxGetNumberOfElements(hulls);
	for (size_t i = 0; i < numPolygons; ++i)
	{
		mxArray* mxFaces = mxGetField(hulls, i, "faces");
		mxArray* mxVerts = mxGetField(hulls, i, "verts");
		mxArray* mxNorms = mxGetField(hulls, i, "norms");
		mxArray* mxColor = mxGetField(hulls, i, "color");
		mxArray* mxFrame = mxGetField(hulls, i, "frame");
		mxArray* mxIndex = mxGetField(hulls, i, "index");
		mxArray* mxlabel = mxGetField(hulls, i, "label");

		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);

		if (numVerts < 1)
			mexErrMsgTxt("No Verts!");

		if (numFaces < 1)
			mexErrMsgTxt("No faces!");

		if (numNormals < 1)
			mexErrMsgTxt("No norms!");

		if (numNormals != numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = int(mxGetScalar(mxFrame)) - 1;

		/*DirectX Side*/
		/*int hullIdx = -1;
		for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
			if (label == (int)mxGetScalar(mxLabel))
			{
				hullIdx = j;
				break;
			}
		}

		SceneNode* parentSceneNode = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->getParentNode();
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->releaseRenderResources();
		delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx];*/
		/*DirectX Side*/
		char buff[255];
		mxGetString(mxlabel, buff, 255);

		gRenderer->convertToScreenSpace(vertData,numVerts);

		QueuePolygon* polygon = new QueuePolygon(numFaces, numVerts, numNormals, frame, (int)mxGetScalar(mxIndex), buff);
		//memcpy(this->pixels, pixels, dimensions.product()* numChannels * numFrames * sizeof(unsigned char));
		polygon->setfaceData(faceData);
		polygon->setvertData(vertData);
		polygon->setnormData(normData);
		polygon->setcolorData(colorData);

		//gRenderer->getMutex();

		/*
		CellHullObject* curHullObj = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0], (float)colorData[1], (float)colorData[2]), 1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx] = new GraphicObjectNode(curHullObj);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->setWireframe(true);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->attachToParentNode(parentSceneNode);
		*/
		//gRenderer->releaseMutex();
		int* intptr = new int;
		*intptr = polygon->getIndex();
		dataQueue->writeMessage("removePolygon", (void*)intptr);
		dataQueue->writeMessage("loadPolygon", (void*)polygon);
	}

	return S_OK;
}

HRESULT addPolygons(const mxArray* polygonsIn)
{
	if (gRenderer == NULL) return E_FAIL;

	size_t numPolygons = mxGetNumberOfElements(polygonsIn);

	vector<QueuePolygon*>* polygons = new vector<QueuePolygon*>(numPolygons);

	for (size_t i = 0; i < numPolygons; ++i)
	{
		mxArray* mxFaces = mxGetField(polygonsIn, i, "faces");
		mxArray* mxVerts = mxGetField(polygonsIn, i, "verts");
		mxArray* mxNorms = mxGetField(polygonsIn, i, "norms");
		mxArray* mxColor = mxGetField(polygonsIn, i, "color");
		mxArray* mxFrame = mxGetField(polygonsIn, i, "frame");
		mxArray* mxIndex = mxGetField(polygonsIn, i, "index");
		mxArray* mxLabel = mxGetField(polygonsIn, i, "label");

		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);

		if (numVerts < 1)
			mexErrMsgTxt("No Verts!");

		if (numFaces < 1)
			mexErrMsgTxt("No faces!");

		if (numNormals < 1)
			mexErrMsgTxt("No norms!");

		if (numNormals != numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = int(mxGetScalar(mxFrame)) - 1;

		char* buff = new char[255];
		mxGetString(mxLabel, buff, 255);

		gRenderer->convertToScreenSpace(vertData,numVerts);

		polygons->at(i) = new QueuePolygon(numFaces, numVerts, numNormals, frame, (int)mxGetScalar(mxIndex), buff);
		polygons->at(i)->setfaceData(faceData);
		polygons->at(i)->setvertData(vertData);
		polygons->at(i)->setnormData(normData);
		polygons->at(i)->setcolorData(colorData);
	}

	dataQueue->writeMessage("loadPolygons", (void*)polygons);

	return S_OK;
}
