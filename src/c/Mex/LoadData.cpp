#include "..\Global\Vec.h"
#include "mex.h"
#include "..\Messages\MessageQueue.h"
#include "windows.h"
#include "..\Global\Globals.h"
#include "MexFunctions.h"
#include <set>
#include "..\Messages\QueuePolygon.h"
#include <vector>

using std::vector;

PolygonObject* createPolygonObject(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals,
	Camera* camera)
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

	PolygonObject* cho = new PolygonObject(gRenderer, faces, verts, normals, camera);

	return cho;
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

		if (numNormals != numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
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

	dataQueue->writeMessage("loadPolygons", (void*)polygons);

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
		faces[2 * i] = VolumeTextureObject::triIndices[0] + 4 * i;
		faces[2 * i + 1] = VolumeTextureObject::triIndices[1] + 4 * i;
	}

	DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale.y, scale.x, scale.z);
	DirectX::XMMATRIX xRot = DirectX::XMMatrixRotationX(DirectX::XM_PI / 2.0f);
	DirectX::XMMATRIX start = DirectX::XMMatrixIdentity();

	for (int i = 0; i < 16; ++i)
	{
		DirectX::XMFLOAT4 curF4(VolumeTextureObject::triVertices[i % 4].x, VolumeTextureObject::triVertices[i % 4].y,
			VolumeTextureObject::triVertices[i % 4].z, 1.0f);

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
		DirectX::XMFLOAT4 curF4(VolumeTextureObject::triVertices[i % 4].x, VolumeTextureObject::triVertices[i % 4].y,
			VolumeTextureObject::triVertices[i % 4].z, 1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, yRot1), scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec), DirectX::XMVectorGetY(rotVec), DirectX::XMVectorGetZ(rotVec));
	}

	for (int i = 20; i < 24; ++i)
	{
		DirectX::XMFLOAT4 curF4(VolumeTextureObject::triVertices[i % 4].x, VolumeTextureObject::triVertices[i % 4].y,
			VolumeTextureObject::triVertices[i % 4].z, 1.0f);

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

	gBorderObj = new PolygonObject(gRenderer, faces, vertices, normals, gCameraDefaultMesh);

	GraphicObjectNode* borderNode = new GraphicObjectNode(gBorderObj);
	gBorderObj->setColor(Vec<float>(0.0f, 0.0f, 0.0f), 1.0f);
	gRenderer->attachToRootScene(borderNode, Renderer::Pre, 0);

	insertGlobalGraphicsObject(GraphicObjectTypes::Border, borderNode);

	//gRenderer->releaseMutex();

	return S_OK;
}

HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ)
{
	if (gRenderer == NULL) return E_FAIL;

	//gRenderer->getMutex();

	unsigned char* shaderConstMemory = NULL;

	GraphicObjectNode* firstVolTextPtr = NULL;
	if (!gGraphicObjectNodes[GraphicObjectTypes::OriginalVolume].empty())
		firstVolTextPtr = gGraphicObjectNodes[GraphicObjectTypes::OriginalVolume].begin()->second;
	else if (!gGraphicObjectNodes[GraphicObjectTypes::ProcessedVolume].empty())
		firstVolTextPtr = gGraphicObjectNodes[GraphicObjectTypes::ProcessedVolume].begin()->second;
	
	if (firstVolTextPtr!=NULL)
		shaderConstMemory = ((VolumeTextureObject*)(firstVolTextPtr->getGraphicObjectPtr()))->getShaderConstMemory();

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

	int fvtIdx = typ-GraphicObjectTypes::OriginalVolume;

	for (int i = 0; i < numFrames; ++i)
	{
		VolumeTextureObject* volumeTexture = new VolumeTextureObject(gRenderer, dims, numChannel, image + i*numChannel*dims.product(), scales,
			gCameraDefaultMesh, &shaderConstMemory);
		shaderConstMemory = volumeTexture->getShaderConstMemory();

		GraphicObjectNode* volumeTextureNode = new GraphicObjectNode(volumeTexture);
		gRenderer->attachToRootScene(volumeTextureNode, Renderer::Section::Main, i);

		insertGlobalGraphicsObject(typ,volumeTextureNode,i);

		if(0==i)
		{
			if(fvtIdx+1 > firstVolumeTextures.size())
				firstVolumeTextures.resize(fvtIdx+1);

			firstVolumeTextures[fvtIdx] = volumeTexture;
		}
	}

	//gRenderer->releaseMutex();

	return S_OK;
}
