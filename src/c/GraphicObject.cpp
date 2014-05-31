#include "GraphicObject.h"

GraphicObject::GraphicObject()
{
	renderer = NULL;
	hasRenderResources = false;
}

GraphicObject::GraphicObject(Renderer* renderer)
{
	this->renderer = renderer;
}

DirectX::XMMATRIX GraphicObject::makeWorldTransform()
{
	DirectX::XMMATRIX worldTrans = DirectX::XMMatrixTranslation(worldPosition.x,worldPosition.y,worldPosition.z);
	DirectX::XMMATRIX worldRot = DirectX::XMMatrixRotationRollPitchYaw(worldRotation.x,worldRotation.y,worldRotation.z);

	return worldRot * worldTrans;
}

GraphicObject::~GraphicObject()
{
	SAFE_DELETE(rendererPackage);
	renderer = NULL;

	hasRenderResources = false;
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
}

CellHullObject::CellHullObject(Renderer* renderer, Camera* camera, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
	std::vector<Vec<float>>& normals) : GraphicObject(renderer)
{
	meshPrimitive = NULL;
	material = NULL;

	this->faces = faces;
	this->vertices = vertices;
	this->normals = normals;
}

HRESULT CellHullObject::addToRenderList(Renderer::Section section, Camera* camera)
{
	if (!hasRenderResources)
	{
		meshPrimitive = renderer->addMeshPrimitive(faces,vertices,normals,Renderer::VertexShaders::Default);
		material = new SingleColoredMaterial(renderer);

		rendererPackage = new RendererPackage(camera);
		rendererPackage->setMeshPrimitive(meshPrimitive);
		rendererPackage->setMaterial(material);
		rendererPackage->setRenderableFlag(true);
	}

	rendererPackage->setLocalToWorld(makeWorldTransform());

	renderer->addToRenderList(rendererPackage,section);

	return S_OK;
}
