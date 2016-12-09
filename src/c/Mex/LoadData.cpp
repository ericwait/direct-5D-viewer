#include "Global/Globals.h"
#include "Global/Vec.h"

#include "Messages/QueuePolygon.h"
#include "Messages/MessageQueue.h"

#include "mex.h"
#include "windows.h"

#include "MexFunctions.h"

#include <set>
#include <vector>
#include <memory>

using std::vector;

std::shared_ptr<MeshPrimitive> createPolygonMesh(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals)
{
	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> verts;
	std::vector<Vec<float>> normals;

	faces.resize(numFaces);
	verts.resize(numVerts);
	normals.resize(numNormals);

	Vec<unsigned int> curFace;
	for (int i = 0; i < numFaces; ++i)
	{
		curFace.x = unsigned int(faceData[i]);
		curFace.y = unsigned int(faceData[i + numFaces]);
		curFace.z = unsigned int(faceData[i + 2 * numFaces]);

		curFace = curFace - 1;

		faces[i] = curFace;
	}

	Vec<float> curVert, curNormal;
	for (int i = 0; i < numVerts; ++i)
	{
		curVert.x = float(vertData[i]);
		curVert.y = float(vertData[i + numVerts]);
		curVert.z = float(vertData[i + 2 * numVerts]);

		curNormal.x = float(normData[i]);
		curNormal.y = float(normData[i + numVerts]);
		curNormal.z = float(normData[i + 2 * numVerts]);

		verts[i] = curVert;
		normals[i] = curNormal;
	}

	return std::make_shared<MeshPrimitive>(gRenderer,Renderer::VertexShaders::DefaultVS, faces, verts, normals);
}


HRESULT loadPolygons(const mxArray* polygonsIn)
{
	size_t numPolygons = mxGetNumberOfElements(polygonsIn);

	vector<QueuePolygon*>* polygons = new vector<QueuePolygon*>(numPolygons);

	for (size_t i = 0; i < numPolygons; ++i)
	{
		// Polygon needs this
		mxArray* mxFaces = mxGetField(polygonsIn, i, "faces");
		mxArray* mxVerts = mxGetField(polygonsIn, i, "verts");
		mxArray* mxNorms = mxGetField(polygonsIn, i, "norms");
		mxArray* mxColor = mxGetField(polygonsIn, i, "color");
		mxArray* mxFrame = mxGetField(polygonsIn, i, "frame");


		mxArray* mxIndex = mxGetField(polygonsIn, i, "index");
		mxArray* mxLabel = mxGetField(polygonsIn, i, "label");

		// Num of each
		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);
		size_t colorDim = mxGetNumberOfElements(mxColor);

		if (mxFaces == NULL)
			mexErrMsgTxt("No faces field!");
		if (mxVerts == NULL)
			mexErrMsgTxt("No verts field!");
		if (mxNorms == NULL)
			mexErrMsgTxt("No norms field!");
		if (mxColor == NULL)
			mexErrMsgTxt("No color field!");
		if (mxFrame == NULL)
			mexErrMsgTxt("No frame field!");
		if (mxIndex == NULL)
			mexErrMsgTxt("No index field!");
		if (mxLabel == NULL)
			mexErrMsgTxt("No label field!");

		if (numVerts < 1)
			mexErrMsgTxt("No verts!");

		if (numFaces < 1)
			mexErrMsgTxt("No faces!");

		if (numNormals < 1)
			mexErrMsgTxt("No norms!");

		if(colorDim<3)
			mexErrMsgTxt("Color is malformed!");

		if (numNormals != numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorDataInit = (double*)mxGetData(mxColor);
		double colorData[4];
		if(colorDim<4)
		{
			memcpy(colorData, colorDataInit, sizeof(double)*3);
			colorData[3] = 1.0;
		}
		else
		{
			memcpy(colorData, colorDataInit, sizeof(double)*4);
		}

		size_t numColor = mxGetNumberOfElements(mxColor);
		int frame = int(mxGetScalar(mxFrame)) - 1;

		char buff[255];
		mxGetString(mxLabel, buff, 255);

		polygons->at(i) = new QueuePolygon(numFaces, numVerts, numNormals, frame, (int)mxGetScalar(mxIndex), buff);
		polygons->at(i)->setfaceData(faceData);
		polygons->at(i)->setvertData(vertData);
		polygons->at(i)->setnormData(normData);
		polygons->at(i)->setcolorData(colorData);
	}

	gMsgQueueToDirectX.writeMessage("loadPolygons", (void*)polygons);

	return S_OK;
}

HRESULT createBorder(Vec<float> &scale)
{
	if (gRenderer == NULL) return E_FAIL;

	//gRenderer->getMutex();

	std::vector<Vec<float>> vertices;
	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> normals;

	faces.resize(12);
	vertices.resize(24);
	normals.resize(24);

	for (int i = 0; i < 6; ++i)
	{
		faces[2 * i] = ViewAlignedPlanes::planeIndices[0] + 4 * i;
		faces[2 * i + 1] = ViewAlignedPlanes::planeIndices[1] + 4 * i;
	}

	DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale.y, scale.x, scale.z);
	DirectX::XMMATRIX xRot = DirectX::XMMatrixRotationX(DirectX::XM_PI / 2.0f);
	DirectX::XMMATRIX start = DirectX::XMMatrixIdentity();

	for (int i = 0; i < 16; ++i)
	{
		DirectX::XMFLOAT4 curF4(ViewAlignedPlanes::planeVertices[i % 4].x, ViewAlignedPlanes::planeVertices[i % 4].y,
			ViewAlignedPlanes::planeVertices[i % 4].z, 1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, start), scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec), DirectX::XMVectorGetY(rotVec), DirectX::XMVectorGetZ(rotVec));
		if (i % 4 == 3)
			start = start*xRot;
	}

	DirectX::XMMATRIX yRot1 = DirectX::XMMatrixRotationY(DirectX::XM_PI / 2.0f);
	DirectX::XMMATRIX yRot2 = DirectX::XMMatrixRotationY(3 * DirectX::XM_PI / 2.0f);

	for (int i = 16; i < 20; ++i)
	{
		DirectX::XMFLOAT4 curF4(ViewAlignedPlanes::planeVertices[i % 4].x, ViewAlignedPlanes::planeVertices[i % 4].y,
			ViewAlignedPlanes::planeVertices[i % 4].z, 1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, yRot1), scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec), DirectX::XMVectorGetY(rotVec), DirectX::XMVectorGetZ(rotVec));
	}

	for (int i = 20; i < 24; ++i)
	{
		DirectX::XMFLOAT4 curF4(ViewAlignedPlanes::planeVertices[i % 4].x, ViewAlignedPlanes::planeVertices[i % 4].y,
			ViewAlignedPlanes::planeVertices[i % 4].z, 1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, yRot2), scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec), DirectX::XMVectorGetY(rotVec), DirectX::XMVectorGetZ(rotVec));
	}

	for (int i = 0; i < 6; ++i)
	{
		Vec<float> edge1, edge2;
		Vec<double> norm;

		edge1 = vertices[faces[2 * i].y] - vertices[faces[2 * i].x];
		edge2 = vertices[faces[2 * i].z] - vertices[faces[2 * i].x];

		Vec<float> triDir = edge1.cross(edge2);

		norm = triDir.norm();

		for (int j = 0; j < 4; ++j)
			normals[faces[2 * i].x + j] = norm;
	}

	std::shared_ptr<MeshPrimitive> borderMesh = std::make_shared<MeshPrimitive>(gRenderer, Renderer::VertexShaders::DefaultVS, faces, vertices, normals);
	std::shared_ptr<SingleColoredMaterial> borderMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.0f, 0.0f, 0.0f), 1.0f);
	GraphicObjectNode* borderNode = new GraphicObjectNode(0, GraphicObjectTypes::Border, borderMesh, borderMat);
	
	borderNode->setLightOn(false);
	gRenderer->attachToRootScene(borderNode, Renderer::Pre, 0);
	insertGlobalGraphicsObject(GraphicObjectTypes::Border, borderNode);

	//gRenderer->releaseMutex();

	return S_OK;
}

HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ)
{
	if (gRenderer == NULL) return E_FAIL;

	//gRenderer->getMutex();

	if (!gGraphicObjectNodes[typ].empty())
	{
		std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[typ].begin();
		for ( ; objectIter != gGraphicObjectNodes[typ].end(); ++objectIter )
		{
			GraphicObjectNode* node = objectIter->second;
			node->detatchFromParentNode();
			delete node;
		}

		gGraphicObjectNodes[typ].clear();
	}

	int volType = typ - GraphicObjectTypes::OriginalVolume;
	if ( !gRenderer->getSharedVolumeParams(volType) )
		gRenderer->createSharedVolumeParams(volType, numChannel);

	std::shared_ptr<StaticVolumeParams>& sharedParams = gRenderer->getSharedVolumeParams(volType);
	std::shared_ptr<ViewAlignedPlanes> planeMesh = std::make_shared<ViewAlignedPlanes>(gRenderer, dims, scales);

	for (int i = 0; i < numFrames; ++i)
	{
		std::shared_ptr<StaticVolumeTextureMaterial> planeMat = std::make_shared<StaticVolumeTextureMaterial>(gRenderer, numChannel, dims, sharedParams);

		GraphicObjectNode* volumeNode = new GraphicObjectNode(i, typ, planeMesh, planeMat);
		for ( int j=0; j < numChannel; ++j )
		{
			const unsigned char* imFrame = image + i*numChannel*dims.product() + j*dims.product();
			volumeNode->getMaterial()->attachTexture(0, std::make_shared<Texture>(gRenderer, dims, imFrame));
		}

		gRenderer->attachToRootScene(volumeNode, Renderer::Section::Main, i);
		insertGlobalGraphicsObject(typ,volumeNode,i);
	}

	//gRenderer->releaseMutex();

	return S_OK;
}
