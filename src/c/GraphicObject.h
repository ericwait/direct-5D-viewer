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
	virtual void setLightOn(bool on){}
	virtual void setWireframe(bool wireframe);

	virtual void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);

	const RendererPackage* getRenderPackage(){return rendererPackage;}
	virtual int getHull(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut);
	virtual int getHullLabel(){return -1;}

protected:
	GraphicObject();

	void setLocalToWorld(DirectX::XMMATRIX localToWorld);
	virtual void updateBoundingBox(DirectX::XMMATRIX localToWorld){}
	Renderer* renderer;
	RendererPackage* rendererPackage;
};


class CellHullObject : public GraphicObject
{
public:
	CellHullObject(Renderer* renderer, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals, Camera* camera);
	~CellHullObject();

	void setLightOn(bool on){}
	void setColor(Vec<float> color, float alpha);
	void setColorMod(Vec<float> colorMod, float alpha);
	void setWireframe(bool wireframe);
	void setLabel(int labelIn);
	void setTrack(int track);
	void getAxisAlignedBoundingBox(Vec<float>& minVals, Vec<float>& maxVals);
	virtual int getHull(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut);
	virtual int getHullLabel(){return label;}

private:
	CellHullObject();
	void initalizeRendererResources(Camera* camera);
	virtual void updateBoundingBox(DirectX::XMMATRIX localToWorld);
	bool intersectTriangle(Vec<unsigned int> face, Vec<float> lclPntVec, Vec<float> lclDirVec, Vec<float>& triCoord);
	MeshPrimitive* meshPrimitive;
	SingleColoredMaterial* material;

	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> vertices;
	std::vector<Vec<float>> normals;
	Vec<float> curBoundingBox[2];
	int label;
};


class VolumeTextureObject : public GraphicObject
{
public:
	static const Vec<unsigned int> triIndices[2]; 
	static const Vec<float> triVertices[4];

	VolumeTextureObject(Renderer* renderer, Vec<size_t> dimsIn, int numChannels, unsigned char* image, Vec<float> scaleFactorIn, Camera* camera,
		unsigned char* constMemIn=NULL);
	~VolumeTextureObject();

	void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);
	unsigned char* getShaderConstMemory(){return material->getShaderConstMemory();}
	void setTransferFunction(int channel, Vec<float> transferFunction){material->setTransferFunction(channel,transferFunction);}
	void setRange(int channel, Vec<float> ranges){material->setRange(channel,ranges);}
	void setColor(int channel, Vec<float> color, float alphaMod){material->setColor(channel,color,alphaMod);}
	void setLightOn(bool on){material->setLightOn(on);}
	void setWireframe(bool wireframe);

	int getNumberOfChannels(){return numChannels;}

private:
	VolumeTextureObject();
	void initalizeRendererResources(Camera* camera, unsigned char* image, unsigned char* shaderMemIn=NULL);

	void createViewAlignedPlanes(std::vector<Vec<float>> &vertices, std::vector<Vec<unsigned int>> &faces, std::vector<Vec<float>> &normals, std::vector<Vec<float>> &textureUVs);

	Vec<size_t> dims;
	Vec<float> scaleFactor;
	int numChannels;
	MeshPrimitive* meshPrimitive;
	StaticVolumeTextureMaterial* material;
};
