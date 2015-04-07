#include "Vec.h"
#include "mex.h"
#include "MessageQueue.h"
#include "windows.h"
#include "Globals.h"
#include "MexFunctions.h"
#include <set>

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

		bool render = i == fvtIdx;
		for (int j = 0; j < gGraphicObjectNodes[idx].size(); ++j)
			gGraphicObjectNodes[idx][j]->setRenderable(render, true);
	}

	gGraphicObjectNodes[GraphicObjectTypes::OriginalVolume][0]->setRenderable(textureType == GraphicObjectTypes::OriginalVolume, false);

	//gRenderer->releaseMutex();
}

void toggleSegmentationResults(bool on)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	for (int i = 0; i < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setRenderable(on, true);

	gGraphicObjectNodes[GraphicObjectTypes::CellHulls][0]->setRenderable(on, false);

	//gRenderer->releaseMutex();
}

void toggleSegmentaionWireframe(bool wireframe)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	for (int i = 0; i < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setWireframe(wireframe);

	//gRenderer->releaseMutex();
}

void toggleSegmentaionLighting(bool lighting)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	for (int i = 0; i < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setLightOn(lighting);

	//gRenderer->releaseMutex();
}

void toggleSelectedCell(std::set<int> labels)
{
	if (gRenderer == NULL) return;

	//gRenderer->getMutex();

	for (int i = 0; i<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
	{
		bool delay = true;

		if (i == gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size() - 1)
			delay = false;

		if (labels.count(gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->getHullLabel())>0)
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setRenderable(true, delay);
		else
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setRenderable(false, delay);
	}

	//gRenderer->releaseMutex();
}

HRESULT updateHulls(const mxArray* hulls)
{
	if (gRenderer == NULL) return E_FAIL;

	size_t numHulls = mxGetNumberOfElements(hulls);
	for (size_t i = 0; i < numHulls; ++i)
	{
		mxArray* mxFaces = mxGetField(hulls, i, "faces");
		mxArray* mxVerts = mxGetField(hulls, i, "verts");
		mxArray* mxNorms = mxGetField(hulls, i, "norms");
		mxArray* mxColor = mxGetField(hulls, i, "color");
		mxArray* mxFrame = mxGetField(hulls, i, "frame");
		mxArray* mxLabel = mxGetField(hulls, i, "label");
		mxArray* mxTrack = mxGetField(hulls, i, "track");

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

		int hullIdx = -1;
		for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
			if (label == (int)mxGetScalar(mxLabel))
			{
				hullIdx = j;
				break;
			}
		}

		//gRenderer->getMutex();

		SceneNode* parentSceneNode = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->getParentNode();
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->releaseRenderResources();
		delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx];

		CellHullObject* curHullObj = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0], (float)colorData[1], (float)colorData[2]), 1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx] = new GraphicObjectNode(curHullObj);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->setWireframe(true);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->attachToParentNode(parentSceneNode);

		//gRenderer->releaseMutex();
	}

	return S_OK;
}

HRESULT addHulls(const mxArray* hulls)
{
	if (gRenderer == NULL) return E_FAIL;

	size_t numHulls = mxGetNumberOfElements(hulls);
	for (size_t i = 0; i < numHulls; ++i)
	{
		mxArray* mxFaces = mxGetField(hulls, i, "faces");
		mxArray* mxVerts = mxGetField(hulls, i, "verts");
		mxArray* mxNorms = mxGetField(hulls, i, "norms");
		mxArray* mxColor = mxGetField(hulls, i, "color");
		mxArray* mxFrame = mxGetField(hulls, i, "frame");
		mxArray* mxLabel = mxGetField(hulls, i, "label");
		mxArray* mxTrack = mxGetField(hulls, i, "track");

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

		int hullIdx = -1;
		for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
			if (label == (int)mxGetScalar(mxLabel))
			{
				hullIdx = j;
				break;
			}
		}

		//gRenderer->getMutex();

		CellHullObject* curHullObj = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0], (float)colorData[1], (float)colorData[2]), 1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		GraphicObjectNode* curHullNode = new GraphicObjectNode(curHullObj);
		curHullNode->setWireframe(true);
		curHullNode->attachToParentNode(hullRootNodes[frame]);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].push_back(curHullNode);

		//gRenderer->releaseMutex();
	}

	return S_OK;
}
