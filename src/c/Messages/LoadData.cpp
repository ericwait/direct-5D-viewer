#include "Global/Globals.h"
#include "Global/Vec.h"
#include "D3d/VolumeInfo.h"
#include "Messages/QueuePolygon.h"
#include "Messages/MessageQueue.h"

#include "windows.h"

#include <set>
#include <vector>
#include <memory>

using std::vector;

std::shared_ptr<MeshPrimitive> createPolygonMesh(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals, const Color& color)
{
	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> verts;
	std::vector<Vec<float>> normals;

	faces.resize(numFaces);
	verts.resize(numVerts);
	normals.resize(numNormals);

	for (int i = 0; i < numFaces; ++i)
	{
		Vec<unsigned int>& curFace = faces[i];

		curFace.x = unsigned int(faceData[i]);
		curFace.y = unsigned int(faceData[i + numFaces]);
		curFace.z = unsigned int(faceData[i + 2 * numFaces]);

		curFace = curFace - 1;
	}

	for (int i = 0; i < numVerts; ++i)
	{
		Vec<float>& curVert = verts[i];

		curVert.x = float(vertData[i]);
		curVert.y = float(vertData[i + numVerts]);
		curVert.z = float(vertData[i + 2 * numVerts]);

		Vec<float>& curNormal = normals[i];

		curNormal.x = float(normData[i]);
		curNormal.y = float(normData[i + numVerts]);
		curNormal.z = float(normData[i + 2 * numVerts]);
	}

	return std::make_shared<StaticColorMesh>(gRenderer, faces, verts, normals, color);
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
		faces[2 * i] = MeshPrimitive::unitQuadIdx[0] + 4 * i;
		faces[2 * i + 1] = MeshPrimitive::unitQuadIdx[1] + 4 * i;
	}

	Eigen::Affine3f placeTfm(Eigen::Translation3f(0.0f, 0.0f, -0.5f));
	Eigen::Affine3f rotXTfm(Eigen::AngleAxisf(M_PI / 2.0f, Eigen::Vector3f::UnitX()));
	Eigen::Affine3f scaleTfm(Eigen::Scaling(scale.y, scale.x, scale.z) * 2.0f);

	// Use rotation about x-axis to get plane vertices for 4 sides of the cube
	Eigen::Affine3f orientTfm = Eigen::Affine3f::Identity();
	for (int i = 0; i < 16; ++i)
	{
		Eigen::Vector4f vert(MeshPrimitive::unitQuadVerts[i % 4].x, MeshPrimitive::unitQuadVerts[i % 4].y, MeshPrimitive::unitQuadVerts[i % 4].z, 1.0f);

		vert = scaleTfm * orientTfm * placeTfm * vert;
		vertices[i] = Vec<float>(vert[0], vert[1], vert[2]);

		if ( i % 4 == 3 )
			orientTfm = rotXTfm * orientTfm;
	}

	// Positive rotation about y-axis for the 5th side
	orientTfm = Eigen::AngleAxisf(M_PI / 2.0f, Eigen::Vector3f::UnitY());
	for (int i = 16; i < 20; ++i)
	{
		Eigen::Vector4f vert(MeshPrimitive::unitQuadVerts[i % 4].x, MeshPrimitive::unitQuadVerts[i % 4].y, MeshPrimitive::unitQuadVerts[i % 4].z, 1.0f);

		vert = scaleTfm * orientTfm * placeTfm * vert;
		vertices[i] = Vec<float>(vert[0], vert[1], vert[2]);
	}


	// Negative rotation about y-axis for the 5th side
	orientTfm = Eigen::AngleAxisf(-M_PI / 2.0f, Eigen::Vector3f::UnitY());
	for (int i = 20; i < 24; ++i)
	{
		Eigen::Vector4f vert(MeshPrimitive::unitQuadVerts[i % 4].x, MeshPrimitive::unitQuadVerts[i % 4].y, MeshPrimitive::unitQuadVerts[i % 4].z, 1.0f);

		vert = scaleTfm * orientTfm * placeTfm * vert;
		vertices[i] = Vec<float>(vert[0], vert[1], vert[2]);
	}

	for (int i = 0; i < 6; ++i)
	{
		Vec<float> edge1, edge2;

		edge1 = vertices[faces[2 * i].e[1]] - vertices[faces[2 * i].e[0]];
		edge2 = vertices[faces[2 * i].e[2]] - vertices[faces[2 * i].e[0]];

		Vec<float> triDir = Vec<float>::cross(edge1,edge2);
		for (int j = 0; j < 4; ++j)
			normals[faces[2 * i].e[0] + j] = triDir.normal();
	}

	std::shared_ptr<MeshPrimitive> borderMesh = std::make_shared<StaticColorMesh>(gRenderer, faces, vertices, normals, Color(0.0f,0.0f,0.0f,1.0f));
	std::shared_ptr<SingleColoredMaterial> borderMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.0f, 0.0f, 0.0f), 1.0f);
	GraphicObjectNode* borderNode = new GraphicObjectNode(0, GraphicObjectTypes::Border, borderMesh, borderMat);
	
	borderNode->setLightOn(false);
	gRenderer->attachToRootScene(borderNode, Renderer::Pre, 0);

	return S_OK;
}

void clearAllTextures(GraphicObjectTypes type)
{
	gRenderer->removeSceneObjects(type);
}

HRESULT initVolume(int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physicalSize, bool columnMajor)
{
	if ( gRenderer == NULL )
		return E_FAIL;

	// Setup volume information
	gRenderer->initVolumeInfo(numFrames, numChannels, dims, physicalSize, columnMajor);

	// Delete old border volumes if they exist
	gRenderer->removeSceneObjects(GraphicObjectTypes::Border);

	HRESULT hr = createBorder(physicalSize/physicalSize.maxValue());
	if ( FAILED(hr) )
		return hr;

	// Clear out old texture graphic object nodes
	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::VTend; ++i )
		clearAllTextures((GraphicObjectTypes)i);

	return S_OK;
}

void clearTextureFrame(int frame, GraphicObjectTypes typ)
{
	GraphicObjectNode* node = gRenderer->findSceneObject(typ, frame);
	if ( node )
		delete node;
}

HRESULT loadTextureFrame(GraphicObjectTypes typ, int frame, unsigned char* image)
{
	if ( gRenderer == NULL )
		return E_FAIL;

	clearTextureFrame(frame, typ);

	const VolumeInfo* info = gRenderer->getVolumeInfo();
	if ( info == NULL )
		return S_FALSE;

	GraphicObjectNode* volumeNode = info->createNode(typ, frame, image);

	gRenderer->attachToRootScene(volumeNode, Renderer::Section::Main, frame);

	return S_OK;
}

HRESULT loadVolumeTexture(unsigned char* image, GraphicObjectTypes typ)
{
	if (gRenderer == NULL)
		return E_FAIL;

	clearAllTextures(typ);

	const VolumeInfo* info = gRenderer->getVolumeInfo();

	int numFrames = info->getFrames();
	int numChannels = info->getChannels();
	Vec<size_t> dims = info->getDims();

	for (int i = 0; i < numFrames; ++i)
	{
		const unsigned char* imFrame = image + i*numChannels*dims.product();
		GraphicObjectNode* volumeNode = info->createNode(typ, i, imFrame);

		gRenderer->attachToRootScene(volumeNode, Renderer::Section::Main, i);
	}

	return S_OK;
}

void attachWidget(double* arrowFaces, size_t numArrowFaces, double* arrowVerts, size_t numArrowVerts, double* arrowNorms, size_t numArrowNorms,
	double* sphereFaces, size_t numSphereFaces, double* sphereVerts, size_t numSphereVerts, double* sphereNorms, size_t numSphereNorms)
{
	SceneNode* widgetScene = new SceneNode(GraphicObjectTypes::Group);
	gRenderer->attachToRootScene(widgetScene, Renderer::Section::Post, 0);

	std::shared_ptr<MeshPrimitive> arrowMeshX = createPolygonMesh(arrowFaces, numArrowFaces, arrowVerts, numArrowVerts, arrowNorms, numArrowNorms, Color(1.0f, 0.2f, 0.2f, 1.0f));
	std::shared_ptr<MeshPrimitive> arrowMeshY = createPolygonMesh(arrowFaces, numArrowFaces, arrowVerts, numArrowVerts, arrowNorms, numArrowNorms, Color(0.1f, 1.0f, 0.1f, 1.0f));
	std::shared_ptr<MeshPrimitive> arrowMeshZ = createPolygonMesh(arrowFaces, numArrowFaces, arrowVerts, numArrowVerts, arrowNorms, numArrowNorms, Color(0.4f, 0.4f, 1.0f, 1.0f));
	std::shared_ptr<MeshPrimitive> sphereMesh = createPolygonMesh(sphereFaces, numSphereFaces, sphereVerts, numSphereVerts, sphereNorms, numSphereNorms, Color(0.9f, 0.9f, 0.9f, 1.0f));

	std::shared_ptr<SingleColoredMaterial> arrowXMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(1.0f, 0.2f, 0.2f), 1.0f);
	std::shared_ptr<SingleColoredMaterial> arrowYMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.1f, 1.0f, 0.1f), 1.0f);
	std::shared_ptr<SingleColoredMaterial> arrowZMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.4f, 0.4f, 1.0f), 1.0f);
	std::shared_ptr<SingleColoredMaterial> sphereMat = std::make_shared<SingleColoredMaterial>(gRenderer, Vec<float>(0.9f, 0.9f, 0.9f), 1.0f);

	GraphicObjectNode* arrowXnode = new GraphicObjectNode(0, GraphicObjectTypes::Widget, arrowMeshX, arrowXMat);
	arrowXnode->setLocalToParent(DirectX::XMMatrixRotationY(DirectX::XM_PI / 2.0f));
	arrowXnode->attachToParentNode(widgetScene);

	GraphicObjectNode* arrowYnode = new GraphicObjectNode(1, GraphicObjectTypes::Widget, arrowMeshY, arrowYMat);
	arrowYnode->setLocalToParent(DirectX::XMMatrixRotationX(-DirectX::XM_PI / 2.0f));
	arrowYnode->attachToParentNode(widgetScene);

	GraphicObjectNode* arrowZnode = new GraphicObjectNode(2, GraphicObjectTypes::Widget, arrowMeshZ, arrowZMat);
	arrowZnode->attachToParentNode(widgetScene);

	GraphicObjectNode* sphereNode = new GraphicObjectNode(3, GraphicObjectTypes::Widget, sphereMesh, sphereMat);
	sphereNode->attachToParentNode(widgetScene);
}

