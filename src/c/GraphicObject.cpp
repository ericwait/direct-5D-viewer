#include "GraphicObject.h"

GraphicObject::GraphicObject()
{
	renderer = NULL;
	rendererPackage = NULL;
}

GraphicObject::GraphicObject(Renderer* renderer)
{
	this->renderer = renderer;
}

GraphicObject::~GraphicObject()
{
	removeRendererResources();
	renderer = NULL;
}

HRESULT GraphicObject::addToRenderList(Renderer::Section section, Camera* camera)
{
	initalizeRendererResources(camera);

	rendererPackage->setCamera(camera);

	renderer->addToRenderList(rendererPackage,section);

	return S_OK;
}

void GraphicObject::removeFromRenderList()
{
	if (rendererPackage!=NULL)
		rendererPackage->setRenderableFlag(false);
}

HRESULT GraphicObject::removeRendererResources()
{
	HRESULT hr = renderer->removeFromRenderList(rendererPackage);
	SAFE_DELETE(rendererPackage);

	return hr;
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

CellHullObject::CellHullObject(Renderer* renderer, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
	std::vector<Vec<float>>& normals) : GraphicObject(renderer)
{
	meshPrimitive = NULL;
	material = NULL;

	this->faces = faces;
	this->vertices = vertices;
	this->normals = normals;
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
