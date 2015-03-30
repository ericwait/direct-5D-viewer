#include "Vec.h"
#include "mex.h"
#include "MessageQueue.h"
#include "windows.h"
#include "Globals.h"
#include "MexFunctions.h"
#include <set>

HRESULT loadWidget(const mxArray* widget[])
{
	if (gRenderer == NULL) return E_FAIL;

	while (!gRendererInit)
		Sleep(10);

	gRenderer->getMutex();

	size_t numFaces = mxGetM(widget[0]);
	size_t numVerts = mxGetM(widget[1]);
	size_t numNormals = mxGetM(widget[2]);

	if (numVerts < 1)
		mexErrMsgTxt("No Verts!");

	if (numFaces < 1)
		mexErrMsgTxt("No faces!");

	if (numNormals < 1)
		mexErrMsgTxt("No norms!");

	double* faceData = (double*)mxGetData(widget[0]);
	double* vertData = (double*)mxGetData(widget[1]);
	double* normData = (double*)mxGetData(widget[2]);

	SceneNode* widgetScene = new SceneNode();
	gRenderer->attachToRootScene(widgetScene, Renderer::Section::Post, 0);

	CellHullObject* arrowX = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraWidget);
	arrowX->setColor(Vec<float>(1.0f, 0.2f, 0.2f), 1.0f);
	GraphicObjectNode* arrowXnode = new GraphicObjectNode(arrowX);
	arrowXnode->setLocalToParent(DirectX::XMMatrixRotationY(DirectX::XM_PI / 2.0f));
	arrowXnode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowXnode);

	CellHullObject* arrowY = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraWidget);
	arrowY->setColor(Vec<float>(0.1f, 1.0f, 0.1f), 1.0f);
	GraphicObjectNode* arrowYnode = new GraphicObjectNode(arrowY);
	arrowYnode->setLocalToParent(DirectX::XMMatrixRotationX(-DirectX::XM_PI / 2.0f));
	arrowYnode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowYnode);

	CellHullObject* arrowZ = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraWidget);
	arrowZ->setColor(Vec<float>(0.4f, 0.4f, 1.0f), 1.0f);
	GraphicObjectNode* arrowZnode = new GraphicObjectNode(arrowZ);
	arrowZnode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowZnode);

	numFaces = mxGetM(widget[3]);
	numVerts = mxGetM(widget[4]);
	numNormals = mxGetM(widget[5]);

	if (numVerts < 1)
		mexErrMsgTxt("No Verts!");

	if (numFaces < 1)
		mexErrMsgTxt("No faces!");

	if (numNormals < 1)
		mexErrMsgTxt("No norms!");

	faceData = (double*)mxGetData(widget[3]);
	vertData = (double*)mxGetData(widget[4]);
	normData = (double*)mxGetData(widget[5]);

	CellHullObject* sphere = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraWidget);
	sphere->setColor(Vec<float>(0.9f, 0.9f, 0.9f), 1.0f);
	GraphicObjectNode* sphereNode = new GraphicObjectNode(sphere);
	sphereNode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(sphereNode);

	gRenderer->releaseMutex();

	return S_OK;
}