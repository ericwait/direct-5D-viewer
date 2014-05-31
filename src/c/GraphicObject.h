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

protected:
	GraphicObject();
	DirectX::XMMATRIX makeWorldTransform();

	Vec<float> worldPosition;
	Vec<float> worldRotation; // Roll, Pitch, Yaw

	Renderer* renderer;
	bool hasRenderResources;
	RendererPackage* rendererPackage;
};

class CellHullObject : public GraphicObject
{
public:
	CellHullObject(Renderer* renderer, Camera* camera, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals);
	~CellHullObject();

	HRESULT addToRenderList(Renderer::Section section, Camera* camera);
	HRESULT removeFromRenderList();
	HRESULT removeRendererResources(){;/*TODO*/}

	void setColor(Vec<float> color);
	void setColor(Vec<float> color, float alpha);
	void setAlpha(float alpha);
	void setColorMod(Vec<float> colorMod);
	void setColorMod(Vec<float> colorMod, float alpha);
	void setAlphaMod(float alphaMod);

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
