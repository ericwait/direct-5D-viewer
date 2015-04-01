#include "Vec.h"
#include "mex.h"
#include "MessageQueue.h"
#include "windows.h"
#include "Globals.h"
#include "MexFunctions.h"
#include <set>

CellHullObject* createCellHullObject(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals,
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

	CellHullObject* cho = new CellHullObject(gRenderer, faces, verts, normals, camera);

	return cho;
}


HRESULT loadHulls(const mxArray* hulls)
{
	if (gRenderer == NULL) return E_FAIL;

	if (!gGraphicObjectNodes[GraphicObjectTypes::CellHulls].empty())
	{
		gRenderer->getMutex();
		for (int j = 0; j < gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->releaseRenderResources();
			delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j];
		}

		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].clear();

		gRenderer->updateRenderList();
		gRenderer->releaseMutex();
	}

	if (hullRootNodes.size() != gRenderer->getNumberOfFrames())
	{
		for (int i = 0; i < hullRootNodes.size(); ++i)
			delete hullRootNodes[i];
	}

	hullRootNodes.resize(gRenderer->getNumberOfFrames());
	for (unsigned int i = 0; i < gRenderer->getNumberOfFrames(); ++i)
		hullRootNodes[i] = new SceneNode();

	size_t numHulls = mxGetNumberOfElements(hulls);
	for (size_t i = 0; i < numHulls; ++i)
	{
		// Polygon needs this
		mxArray* mxFaces = mxGetField(hulls, i, "faces");
		mxArray* mxVerts = mxGetField(hulls, i, "verts");
		mxArray* mxNorms = mxGetField(hulls, i, "norms");
		mxArray* mxColor = mxGetField(hulls, i, "color");
		mxArray* mxFrame = mxGetField(hulls, i, "frame");


		mxArray* mxLabel = mxGetField(hulls, i, "label");
		mxArray* mxTrack = mxGetField(hulls, i, "track");

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
		if (mxLabel == NULL)
			mexErrMsgTxt("No label field!");
		if (mxTrack == NULL)
			mexErrMsgTxt("No track field!");

		if (numVerts < 1)
			mexErrMsgTxt("No verts!");

		if (numFaces < 1)
			mexErrMsgTxt("No faces!");

		if (numNormals < 1)
			mexErrMsgTxt("No norms!");

		if (numNormals != numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		// Store data like this
		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = int(mxGetScalar(mxFrame)) - 1;
		// end class

		gRenderer->getMutex();

		CellHullObject* curHullObj = createCellHullObject(faceData, numFaces, vertData, numVerts, normData, numNormals, gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0], (float)colorData[1], (float)colorData[2]), 1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		GraphicObjectNode* curHullNode = new GraphicObjectNode(curHullObj);
		curHullNode->setWireframe(true);
		curHullNode->attachToParentNode(hullRootNodes[frame]);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].push_back(curHullNode);

		gRenderer->releaseMutex();
	}

	gRenderer->getMutex();

	for (unsigned int i = 0; i < gRenderer->getNumberOfFrames(); ++i)
		gRenderer->attachToRootScene(hullRootNodes[i], Renderer::Section::Main, i);

	gRenderer->releaseMutex();

	return S_OK;
}

HRESULT createBorder(Vec<float> &scale)
{
	if (gRenderer == NULL) return E_FAIL;

	gRenderer->getMutex();

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

	gBorderObj = new CellHullObject(gRenderer, faces, vertices, normals, gCameraDefaultMesh);

	GraphicObjectNode* borderNode = new GraphicObjectNode(gBorderObj);
	gBorderObj->setColor(Vec<float>(0.0f, 0.0f, 0.0f), 1.0f);
	gRenderer->attachToRootScene(borderNode, Renderer::Pre, 0);

	gGraphicObjectNodes[GraphicObjectTypes::Border].push_back(borderNode);

	gRenderer->releaseMutex();

	return S_OK;
}

HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ)
{
	if (gRenderer == NULL) return E_FAIL;

	gRenderer->getMutex();

	unsigned char* shaderConstMemory = NULL;

	int fvtIdx = typ - GraphicObjectTypes::OriginalVolume;

	if (!firstVolumeTextures.empty() && fvtIdx < firstVolumeTextures.size() && NULL != firstVolumeTextures[fvtIdx])
	{
		for (int i = 0; i < gGraphicObjectNodes[typ].size(); ++i)
		{
			delete gGraphicObjectNodes[typ][i];
		}

		gGraphicObjectNodes[typ].clear();
	}

	for (int i = 0; i < numFrames; ++i)
	{
		VolumeTextureObject* volumeTexture = new VolumeTextureObject(gRenderer, dims, numChannel, image + i*numChannel*dims.product(), scales,
			gCameraDefaultMesh, shaderConstMemory);
		shaderConstMemory = volumeTexture->getShaderConstMemory();

		GraphicObjectNode* volumeTextureNode = new GraphicObjectNode(volumeTexture);
		gRenderer->attachToRootScene(volumeTextureNode, Renderer::Section::Main, i);

		gGraphicObjectNodes[typ].push_back(volumeTextureNode);

		if (0 == i)
		{
			if (fvtIdx + 1 > firstVolumeTextures.size())
				firstVolumeTextures.resize(fvtIdx + 1);

			firstVolumeTextures[fvtIdx] = volumeTexture;
		}
	}

	gRenderer->releaseMutex();

	return S_OK;
}