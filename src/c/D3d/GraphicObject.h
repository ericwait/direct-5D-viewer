////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Global/Vec.h"
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

	void removeFromRenderList() {}
	void removeRendererResources();
	virtual void setLightOn(bool on){}
	virtual void setWireframe(bool wireframe);

	virtual void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);

	const RendererPackage* getRenderPackage(){return rendererPackage;}
	virtual int getPolygon(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut);
	virtual int getIndex(){return -1;}

protected:
	GraphicObject();

	void setLocalToWorld(DirectX::XMMATRIX localToWorld);
	virtual void updateBoundingBox(DirectX::XMMATRIX localToWorld){}
	Renderer* renderer;
	RendererPackage* rendererPackage;
};


class PolygonObject : public GraphicObject
{
public:
	PolygonObject(Renderer* renderer, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals, Camera* camera);
	~PolygonObject();

	void setLightOn(bool on) { material->setLightOn(on); }
	void setColor(Vec<float> color, float alpha);
	void setColorMod(Vec<float> colorMod, float alpha);
	void setWireframe(bool wireframe);
	void setIndex(int labelIn);
	void setLabel(std::string label);
	void getAxisAlignedBoundingBox(Vec<float>& minVals, Vec<float>& maxVals);
	virtual int getPolygon(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut);
	virtual int getIndex(){return index;}

private:
	PolygonObject();
	void initalizeRendererResources(Camera* camera);
	virtual void updateBoundingBox(DirectX::XMMATRIX localToWorld);
	bool intersectTriangle(Vec<unsigned int> face, Vec<float> lclPntVec, Vec<float> lclDirVec, Vec<float>& triCoord);
	MeshPrimitive* meshPrimitive;
	SingleColoredMaterial* material;

	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> vertices;
	std::vector<Vec<float>> normals;
	Vec<float> curBoundingBox[2];
	int index;
};


class VolumeTextureObject : public GraphicObject
{
public:
	static const Vec<unsigned int> triIndices[2]; 
	static const Vec<float> triVertices[4];

	VolumeTextureObject(Renderer* renderer, Vec<size_t> dimsIn, int numChannels, unsigned char* image, Vec<float> scaleFactorIn, Camera* camera,
		unsigned char** constMemIn=NULL);
	~VolumeTextureObject();

	void makeLocalToWorld(DirectX::XMMATRIX parentToWorld);
	unsigned char* getShaderConstMemory(){return material->getShaderConstMemory();}
	void setTransferFunction(int channel, Vec<float> transferFunction){material->setTransferFunction(channel,transferFunction);}
	void setRange(int channel, Vec<float> ranges){material->setRange(channel,ranges);}
	void setColor(int channel, Vec<float> color, float alphaMod){material->setColor(channel,color,alphaMod);}
	void setLightOn(bool on){material->setLightOn(on);}
	void setAttenuationOn(bool on){ material->setAttenuationOn(on); }
	void setWireframe(bool wireframe);

	int getNumberOfChannels(){return numChannels;}
	Vec<size_t> getDims() { return dims; }
	Vec<float> getScales() { return scaleFactor; }
	Vec<float> getPhysVolSize() { return physVolSize; }

private:
	VolumeTextureObject();
	void initalizeRendererResources(Camera* camera, unsigned char* image, unsigned char** shaderMemIn=NULL);

	void createViewAlignedPlanes(std::vector<Vec<float>> &vertices, std::vector<Vec<unsigned int>> &faces, std::vector<Vec<float>> &normals, std::vector<Vec<float>> &textureUVs);

	Vec<size_t> dims;
	Vec<float> scaleFactor;
	Vec<float> physVolSize;
	int numChannels;
	MeshPrimitive* meshPrimitive;
	StaticVolumeTextureMaterial* material;
};
