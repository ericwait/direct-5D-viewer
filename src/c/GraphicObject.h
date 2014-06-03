#pragma once
#include "Vec.h"
#include "MeshPrimitive.h"
#include "Material.h"
#include "RendererPackage.h"
#include "Camera.h"
#include <vector>

class GraphicObject
{
public:
	GraphicObject(Renderer* renderer);
	virtual ~GraphicObject();

	HRESULT addToRenderList(Renderer::Section section, Camera* camera);
	void removeFromRenderList();
	HRESULT removeRendererResources();

	virtual void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);

protected:
	GraphicObject();

	void setLocalToWorld(DirectX::XMMATRIX localToWorld);
	virtual void initalizeRendererResources(Camera* camera) = 0;

	Renderer* renderer;
	RendererPackage* rendererPackage;
};

class CellHullObject : public GraphicObject
{
public:
	CellHullObject(Renderer* renderer, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals);
	~CellHullObject();

	void setColor(Vec<float> color, float alpha);
	void setColorMod(Vec<float> colorMod, float alpha);

protected:
		void initalizeRendererResources(Camera* camera);

private:
	CellHullObject();
	MeshPrimitive* meshPrimitive;
	SingleColoredMaterial* material;

	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> vertices;
	std::vector<Vec<float>> normals;
};

// class VolumeTextureObject : GraphicObject
// {
// public:
// 	VolumeTextureObject();
// 	~VolumeTextureObject();
// 
// private:
// 	MeshPrimitive* meshPrimitive;
// 	//TODO list of materials
// };
