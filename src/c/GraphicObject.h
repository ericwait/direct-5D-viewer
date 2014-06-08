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
	friend class GraphicObjectNode;
	GraphicObject(Renderer* renderer);
	virtual ~GraphicObject();

	void removeFromRenderList();
	void removeRendererResources();

	virtual void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);

	const RendererPackage* getRenderPackage(){return rendererPackage;}

protected:
	GraphicObject();

	void setLocalToWorld(DirectX::XMMATRIX localToWorld);

	Renderer* renderer;
	RendererPackage* rendererPackage;

};

class CellHullObject : public GraphicObject
{
public:
	CellHullObject(Renderer* renderer, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals, Camera* camera);
	~CellHullObject();

	void setColor(Vec<float> color, float alpha);
	void setColorMod(Vec<float> colorMod, float alpha);

private:
	CellHullObject();
	void initalizeRendererResources(Camera* camera);

	MeshPrimitive* meshPrimitive;
	SingleColoredMaterial* material;

	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> vertices;
	std::vector<Vec<float>> normals;
};

class VolumeTextureObject : public GraphicObject
{
public:
	static const Vec<unsigned int> triIndices[2]; 

	static const Vec<float> triVertices[4];

	VolumeTextureObject(Renderer* renderer, Vec<size_t> dimsIn, int numChannels, unsigned char* image, Vec<float> scaleFactorIn, Camera* camera);
	~VolumeTextureObject();

	void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);

private:
	VolumeTextureObject();
	void initalizeRendererResources(Camera* camera, unsigned char* image);

	void createViewAlignedPlanes(std::vector<Vec<float>> &vertices, std::vector<Vec<unsigned int>> &faces, std::vector<Vec<float>> &normals, std::vector<Vec<float>> &textureUVs);

	Vec<size_t> dims;
	Vec<float> scaleFactor;
	int numChannels;
	MeshPrimitive* meshPrimitive;
	StaticVolumeTextureMaterial* material;
};

