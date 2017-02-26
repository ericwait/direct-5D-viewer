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
#include "VertexLayouts.h"
#include "Renderer.h"

#include "Global/Vec.h"
#include "Global/Color.h"

#include <d3d11.h>
#include <cstdint>
#include <vector>
#include <string>

class MeshPrimitive
{
	friend class Renderer;
	friend class GraphicObjectNode;

public:
	static const Vec<uint32_t> unitQuadIdx[2];
	static const Vec<float> unitQuadVerts[4];
	static const Vec<float> unitQuadNorms[4];

	bool checkIntersect(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut);

	~MeshPrimitive();
protected:
	MeshPrimitive(Renderer* rendererIn, VertexLayout::Types layoutType = VertexLayout::Types::P,
		const std::string& shaderFile = "DefaultMeshShaders", const std::string& shaderFunc = "DefaultMeshVertexShader");

	MeshPrimitive(Renderer* rendererIn,
		const std::vector<Vec<uint32_t>>& faces, const std::vector<Vec<float>>& vertices,
		const std::vector<Vec<float>>& normals = std::vector<Vec<float>>(),
		const std::vector<Vec<float>>& textureUV = std::vector<Vec<float>>(),
		const std::vector<Color>& colors = std::vector<Color>());	

	void setupMesh(const std::vector<Vec<uint32_t>>& facesIn, const std::vector<Vec<float>>& verticesIn,
		const std::vector<Vec<float>>& normalsIn = std::vector<Vec<float>>(),
		const std::vector<Vec<float>>& texUVsIn = std::vector<Vec<float>>(),
		const std::vector<Color>& colorsIn = std::vector<Color>());

	void cleanupMesh();

	void loadShader(const std::string& shaderFile, const std::string& shaderFunc);
	void initializeResources(UINT vertAccessFlags = 0, UINT indexAccessFlags = 0);
	void updateCenterOfMass();

	Vec<float> getCenterOfMass() const {return centerOfMass;}
	bool intersectTriangle(Vec<uint32_t> face, Vec<float> lclPntVec, Vec<float> lclDirVec, Vec<float>& triCoord);

	// TODO: This really should probably be part of the material system
	// Overloaded to change the way transforms get passed to the vertex shader
	virtual DirectX::XMMATRIX computeLocalToWorld(DirectX::XMMATRIX parentToWorld){return parentToWorld;}

private:
	MeshPrimitive():layout(VertexLayout::Types::P){}

protected:

	Renderer* renderer;

	size_t numFaces;
	size_t numVerts;
	Vec<float> centerOfMass;

	// Triangle resources
	std::vector<Vec<uint32_t>> faces;
	std::vector<Vec<float>> vertices;
	std::vector<Vec<float>> normals;
	std::vector<Vec<float>> texUVs;
	std::vector<Color> colors;
	std::vector<Color> backColors;

	// Render resources
	int vertShaderIdx;

	VertexLayout layout;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};


class StaticColorMesh : public MeshPrimitive
{
public:
	StaticColorMesh(Renderer* renderer, const std::vector<Vec<uint32_t>>& faces, const std::vector<Vec<float>>& vertices,
		const std::vector<Vec<float>>& normals, const Color& color);

	StaticColorMesh(Renderer* renderer, const std::vector<Vec<uint32_t>>& faces, const std::vector<Vec<float>>& vertices,
		const std::vector<Vec<float>>& normals, const std::vector<Color>& colors);
};


class TextQuads : public MeshPrimitive
{
public:
	TextQuads(Renderer* renderer, size_t maxQuads);

	void clearQuads();
	bool addQuad(const Vec<float> pos[4], const Vec<float> uv[4], const Color& color, const Color& backColor);

	void updateResources();

private:

	int maxQuads;
	int maxFaces;
};


class ViewAlignedPlanes : public MeshPrimitive
{
public:
	ViewAlignedPlanes(Renderer* renderer, Vec<size_t> volDims, Vec<float> scaleDims);

protected:
	virtual DirectX::XMMATRIX computeLocalToWorld(DirectX::XMMATRIX parentToWorld);

private:
	void buildViewAlignedPlanes(Vec<size_t> volDims, std::vector<Vec<uint32_t>>& faces,
		std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& textureUV);

	// Need to keep these around for transforms
	// TODO: Put all shader refs together into the material?
	Vec<size_t> dims;

	// This is the size of the texture in physical space
	Vec<float> physicalSize;
};
