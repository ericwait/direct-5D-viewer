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
#include "Renderer.h"

#include <d3d11.h>
#include <vector>

class MeshPrimitive
{
public:
	friend class Renderer;
	friend class GraphicObjectNode;

	MeshPrimitive(Renderer* rendererIn, Renderer::VertexShaders shader, const std::vector<Vec<unsigned int>>& faces,const std::vector<Vec<float>>& vertices,
		const std::vector<Vec<float>>& normals, const std::vector<Vec<float>>& textureUV = std::vector<Vec<float>>());

	bool checkIntersect(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut);

	~MeshPrimitive();
protected:
	MeshPrimitive(Renderer* rendererIn, Renderer::VertexShaders shader);

	void setupMesh(const std::vector<Vec<unsigned int>>& faces, const std::vector<Vec<float>>& vertices,
		const std::vector<Vec<float>>& normals, const std::vector<Vec<float>>& textureUV = std::vector<Vec<float>>());

	void cleanupMesh();

	void loadShader(Renderer::VertexShaders shader);
	void initializeResources(const std::vector<Vec<float>>& textureUV);
	void updateCenterOfMass();

	Vec<float> getCenterOfMass() const {return centerOfMass;}
	bool intersectTriangle(Vec<unsigned int> face, Vec<float> lclPntVec, Vec<float> lclDirVec, Vec<float>& triCoord);

	// TODO: This really should probably be part of the material system
	// Overloaded to change the way transforms get passed to the vertex shader
	virtual DirectX::XMMATRIX computeLocalToWorld(DirectX::XMMATRIX parentToWorld){return parentToWorld;}

private:
	MeshPrimitive(){}

protected:
	Renderer* renderer;

	size_t numFaces;
	Vec<float> centerOfMass;

	// Triangle resources
	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> vertices;
	std::vector<Vec<float>> normals;

	// Render resources
	int vertShaderIdx;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};


class ViewAlignedPlanes : public MeshPrimitive
{
public:
	// Static indices/verts for planes
	static const Vec<unsigned int> planeIndices[2];
	static const Vec<float> planeVertices[4];

	ViewAlignedPlanes(Renderer* renderer, Vec<size_t> volDims, Vec<float> scaleDims);

protected:
	virtual DirectX::XMMATRIX computeLocalToWorld(DirectX::XMMATRIX parentToWorld);

private:
	void buildViewAlignedPlanes(Vec<size_t> volDims, std::vector<Vec<unsigned int>>& faces,
		std::vector<Vec<float>>& vertices,std::vector<Vec<float>>& normals, std::vector<Vec<float>>& textureUV);

	// Need to keep these around for transforms
	// TODO: Put all shader refs together into the material?
	Vec<size_t> dims;

    // This is the size of the texture in physical space
	Vec<float> physicalSize;
};