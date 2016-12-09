#include "Global/Vec.h"
#include "Global/Globals.h"

#include "Messages/MessageQueue.h"
#include "MexFunctions.h"

#include "mex.h"
#include <windows.h>

#include <set>

HRESULT loadWidget(const mxArray* widget[])
{
	if (gRenderer == NULL) return E_FAIL;

	while (!gRendererInit)
		Sleep(10);

	//gRenderer->getMutex();

	size_t numArrowFaces = mxGetM(widget[0]);
	size_t numArrowVerts = mxGetM(widget[1]);
	size_t numArrowNormals = mxGetM(widget[2]);

	size_t numSphereFaces = mxGetM(widget[3]);
	size_t numSphereVerts = mxGetM(widget[4]);
	size_t numSphereNormals = mxGetM(widget[5]);

	if (numArrowVerts < 1)
		mexErrMsgTxt("No Verts!");

	if (numArrowFaces < 1)
		mexErrMsgTxt("No faces!");

	if (numArrowNormals < 1)
		mexErrMsgTxt("No norms!");

	if (numSphereVerts < 1)
		mexErrMsgTxt("No Verts!");

	if (numSphereFaces < 1)
		mexErrMsgTxt("No faces!");

	if (numSphereNormals < 1)
		mexErrMsgTxt("No norms!");

	double* arrowFaces = (double*)mxGetData(widget[0]);
	double* arrowVerts = (double*)mxGetData(widget[1]);
	double* arrowNorms = (double*)mxGetData(widget[2]);

	double* sphereFaces = (double*)mxGetData(widget[3]);
	double* sphereVerts = (double*)mxGetData(widget[4]);
	double* sphereNorms = (double*)mxGetData(widget[5]);


	SceneNode* widgetScene = new SceneNode(GraphicObjectTypes::Group);
	gRenderer->attachToRootScene(widgetScene, Renderer::Section::Post, 0);

	std::shared_ptr<MeshPrimitive> arrowMesh = createPolygonMesh(arrowFaces, numArrowFaces, arrowVerts, numArrowVerts, arrowNorms, numArrowNormals);
	std::shared_ptr<MeshPrimitive> sphereMesh = createPolygonMesh(sphereFaces, numSphereFaces, sphereVerts, numSphereVerts, sphereNorms, numSphereNormals);

	std::shared_ptr<SingleColoredMaterial> arrowXMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(1.0f, 0.2f, 0.2f), 1.0f);
	std::shared_ptr<SingleColoredMaterial> arrowYMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.1f, 1.0f, 0.1f), 1.0f);
	std::shared_ptr<SingleColoredMaterial> arrowZMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.4f, 0.4f, 1.0f), 1.0f);
	std::shared_ptr<SingleColoredMaterial> sphereMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.9f, 0.9f, 0.9f), 1.0f);

	GraphicObjectNode* arrowXnode = new GraphicObjectNode(0, GraphicObjectTypes::Widget, arrowMesh, arrowXMat);
	arrowXnode->setLocalToParent(DirectX::XMMatrixRotationY(DirectX::XM_PI / 2.0f));
	arrowXnode->attachToParentNode(widgetScene);
	insertGlobalGraphicsObject(GraphicObjectTypes::Widget, arrowXnode);

	GraphicObjectNode* arrowYnode = new GraphicObjectNode(1, GraphicObjectTypes::Widget, arrowMesh, arrowYMat);
	arrowYnode->setLocalToParent(DirectX::XMMatrixRotationX(-DirectX::XM_PI / 2.0f));
	arrowYnode->attachToParentNode(widgetScene);
	insertGlobalGraphicsObject(GraphicObjectTypes::Widget, arrowYnode);

	GraphicObjectNode* arrowZnode = new GraphicObjectNode(2, GraphicObjectTypes::Widget, arrowMesh, arrowZMat);
	arrowZnode->attachToParentNode(widgetScene);
	insertGlobalGraphicsObject(GraphicObjectTypes::Widget, arrowZnode);

	GraphicObjectNode* sphereNode = new GraphicObjectNode(3, GraphicObjectTypes::Widget, sphereMesh, sphereMat);
	sphereNode->attachToParentNode(widgetScene);
	insertGlobalGraphicsObject(GraphicObjectTypes::Widget, sphereNode);

	//gRenderer->releaseMutex();

	return S_OK;
}