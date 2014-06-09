#include "GraphicObject.h"

const Vec<unsigned int> VolumeTextureObject::triIndices[2] = 
{
	Vec<unsigned int>(0,2,1),
	Vec<unsigned int>(1,2,3)
};
const Vec<float> VolumeTextureObject::triVertices[4] = 
{
	Vec<float>( -1.0f, -1.0f, 1.0f ), 
	Vec<float>( -1.0f, 1.0f, 1.0f), 
	Vec<float>( 1.0f, -1.0f, 1.0f), 
	Vec<float>( 1.0f, 1.0f, 1.0f)
};

GraphicObject::GraphicObject()
{
	renderer = NULL;
	rendererPackage = NULL;
}

GraphicObject::GraphicObject(Renderer* renderer)
{
	this->renderer = renderer;
	rendererPackage = NULL;
}

GraphicObject::~GraphicObject()
{
	removeRendererResources();
	renderer = NULL;
}

void GraphicObject::removeFromRenderList()
{
	if (rendererPackage!=NULL)
		rendererPackage->setRenderableFlag(false);
}

void GraphicObject::removeRendererResources()
{
	SAFE_DELETE(rendererPackage);
}

void GraphicObject::setLocalToWorld(DirectX::XMMATRIX localToWorld)
{
	if (rendererPackage!=NULL)
		rendererPackage->setLocalToWorld(localToWorld);
}

void GraphicObject::makeLocalToWorld(DirectX::XMMATRIX parentToWorld)
{
	setLocalToWorld(parentToWorld);
}

CellHullObject::CellHullObject()
{
	meshPrimitive = NULL;
	material = NULL;
}

CellHullObject::~CellHullObject()
{
	//TODO remove meshPrimitive or just set to NULL

	SAFE_DELETE(material);

	faces.clear();
	vertices.clear();
	normals.clear();

	GraphicObject::~GraphicObject();
}

CellHullObject::CellHullObject(Renderer* rendererIn, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
	std::vector<Vec<float>>& normals, Camera* camera)
{
	meshPrimitive = NULL;
	material = NULL;

	renderer = rendererIn;
	this->faces = faces;
	this->vertices = vertices;
	this->normals = normals;

	initalizeRendererResources(camera);
}


void CellHullObject::initalizeRendererResources(Camera* camera)
{
	if (rendererPackage==NULL)
	{
		meshPrimitive = renderer->addMeshPrimitive(faces,vertices,normals,Renderer::VertexShaders::Default);
		material = new SingleColoredMaterial(renderer);

		rendererPackage = new RendererPackage(camera);
		rendererPackage->setMeshPrimitive(meshPrimitive);
		rendererPackage->setMaterial(material);
		rendererPackage->setRenderableFlag(true);
	}
}

void CellHullObject::setColor(Vec<float> color, float alpha)
{
	if (material!=NULL)
		material->setColor(color,alpha);
}

void CellHullObject::setColorMod(Vec<float> colorMod, float alpha)
{
	if (material!=NULL)
		material->setColorModifier(colorMod,alpha);
}

VolumeTextureObject::VolumeTextureObject(Renderer* rendererIn, Vec<size_t> dimsIn, int numChannelsIn, unsigned char* image, 
										Vec<float> scaleFactorIn, Camera* camera, unsigned char* constMemIn/*=NULL*/)
{
	renderer = rendererIn;
	dims = dimsIn;
	numChannels = numChannelsIn;
	scaleFactor = scaleFactorIn;

	initalizeRendererResources(camera,image, constMemIn);
}

VolumeTextureObject::~VolumeTextureObject()
{
	SAFE_DELETE(material);

	GraphicObject::~GraphicObject();
}

void VolumeTextureObject::initalizeRendererResources(Camera* camera, unsigned char* image, unsigned char* shaderMemIn/*=NULL*/)
{
	if (rendererPackage==NULL)
	{
		std::vector<Vec<unsigned int>> faces;
		std::vector<Vec<float>> vertices;
		std::vector<Vec<float>> normals;
		std::vector<Vec<float>> textureUVs;

		createViewAlignedPlanes(vertices, faces, normals, textureUVs);

		meshPrimitive = renderer->addMeshPrimitive(faces,vertices,normals,textureUVs,Renderer::VertexShaders::ViewAligned);
		material = new StaticVolumeTextureMaterial(renderer,dims,numChannels,image,shaderMemIn);

		rendererPackage = new RendererPackage(camera);
		rendererPackage->setMeshPrimitive(meshPrimitive);
		rendererPackage->setMaterial(material);
		rendererPackage->setRenderableFlag(true);
	}
}

void VolumeTextureObject::makeLocalToWorld(DirectX::XMMATRIX parentToWorld)
{
	DirectX::XMVECTOR det;
	DirectX::XMMATRIX invParentWorld = DirectX::XMMatrixInverse(&det,parentToWorld);

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(-0.5f, -0.5f, -0.5f) //centering texture coord at vol origin
		* invParentWorld 
		//* DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f) //move up to trans?
		* DirectX::XMMatrixScaling(1.0f/scaleFactor.x, 1.0f/scaleFactor.y, 1.0f/scaleFactor.z) //convert space from world to local
		* DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.5f); //puts the origin back to 0

	setLocalToWorld(worldMatrix);
}

void VolumeTextureObject::createViewAlignedPlanes(std::vector<Vec<float>> &vertices, std::vector<Vec<unsigned int>> &faces,
	std::vector<Vec<float>> &normals, std::vector<Vec<float>> &textureUVs)
{
	int numPlanes = dims.maxValue() * 1.5;

	vertices.resize(4*numPlanes);
	faces.resize(2*numPlanes);
	normals.resize(4*numPlanes);
	textureUVs.resize(4*numPlanes);

	int planesFirstVert = 0;
	for (int planeIdx=0; planeIdx<numPlanes; ++planeIdx)
	{
		float zPosition = (2.0f*planeIdx) / numPlanes -1.0f;

		//i is making each plane
		for (int i=0; i<4; i++)
		{
			vertices[planesFirstVert+i] = triVertices[i]*1.5f;
			vertices[planesFirstVert+i].z = zPosition*1.5f;

			textureUVs[planesFirstVert+i] = vertices[planesFirstVert+i] * 0.5f + 0.5f;
		}

		for (int i=0; i<2; ++i)
			faces[2*(numPlanes-planeIdx-1) + i] = triIndices[i] + planesFirstVert;

		Vec<float> edge1, edge2;
		Vec<double> norm;

		edge1 = vertices[faces[2*planeIdx].y] - vertices[faces[2*planeIdx].x];
		edge2 = vertices[faces[2*planeIdx].z] - vertices[faces[2*planeIdx].x];

		Vec<float> triDir = edge1.cross(edge2);

		norm = triDir.norm();

		for (int i=0; i<4 ; ++i)
			normals[planesFirstVert+i] = norm;

		planesFirstVert += 4;
	}
}
