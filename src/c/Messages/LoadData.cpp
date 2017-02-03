#include "Global/Globals.h"
#include "Global/Vec.h"
#include "Messages/QueuePolygon.h"
#include "Messages/MessageQueue.h"

#include "windows.h"

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

	return std::make_shared<MeshPrimitive>(gRenderer, faces, verts, normals);
}


HRESULT createBorder(Vec<float> &scale)
{
	if (gRenderer == NULL)
		return E_FAIL;

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
		Vec<float> norm;

		edge1 = vertices[faces[2 * i].y] - vertices[faces[2 * i].x];
		edge2 = vertices[faces[2 * i].z] - vertices[faces[2 * i].x];

		Vec<float> triDir = Vec<float>::cross(edge1,edge2);

		norm = triDir.normal();

		for (int j = 0; j < 4; ++j)
			normals[faces[2 * i].x + j] = norm;
	}

	std::shared_ptr<MeshPrimitive> borderMesh = std::make_shared<MeshPrimitive>(gRenderer, faces, vertices, normals);
	std::shared_ptr<SingleColoredMaterial> borderMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.0f, 0.0f, 0.0f), 1.0f);
	GraphicObjectNode* borderNode = new GraphicObjectNode(0, GraphicObjectTypes::Border, borderMesh, borderMat);
	
	borderNode->setLightOn(false);
	gRenderer->attachToRootScene(borderNode, Renderer::Pre, 0);
	insertGlobalGraphicsObject(GraphicObjectTypes::Border, borderNode);

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

	std::shared_ptr<StaticVolumeParams>& sharedParams = std::dynamic_pointer_cast<StaticVolumeParams>(gRenderer->getSharedVolumeParams(volType));
	std::shared_ptr<ViewAlignedPlanes> planeMesh = std::make_shared<ViewAlignedPlanes>(gRenderer, dims, scales);

	for (int i = 0; i < numFrames; ++i)
	{
		std::shared_ptr<StaticVolumeTextureMaterial> planeMat = std::make_shared<StaticVolumeTextureMaterial>(gRenderer, numChannel, dims, sharedParams);

		GraphicObjectNode* volumeNode = new GraphicObjectNode(i, typ, planeMesh, planeMat);
		for ( int j=0; j < numChannel; ++j )
		{
			const unsigned char* imFrame = image + i*numChannel*dims.product() + j*dims.product();
			volumeNode->getMaterial()->attachTexture(j, std::make_shared<ConstTexture>(gRenderer, dims, imFrame));
		}

		gRenderer->attachToRootScene(volumeNode, Renderer::Section::Main, i);
		insertGlobalGraphicsObject(typ,volumeNode,i);
	}

	//gRenderer->releaseMutex();

	return S_OK;
}

void attachWidget(double* arrowFaces, size_t numArrowFaces, double* arrowVerts, size_t numArrowVerts, double* arrowNorms, size_t numArrowNorms,
	double* sphereFaces, size_t numSphereFaces, double* sphereVerts, size_t numSphereVerts, double* sphereNorms, size_t numSphereNorms)
{
	SceneNode* widgetScene = new SceneNode(GraphicObjectTypes::Group);
	gRenderer->attachToRootScene(widgetScene, Renderer::Section::Post, 0);

	std::shared_ptr<MeshPrimitive> arrowMesh = createPolygonMesh(arrowFaces, numArrowFaces, arrowVerts, numArrowVerts, arrowNorms, numArrowNorms);
	std::shared_ptr<MeshPrimitive> sphereMesh = createPolygonMesh(sphereFaces, numSphereFaces, sphereVerts, numSphereVerts, sphereNorms, numSphereNorms);

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
}

