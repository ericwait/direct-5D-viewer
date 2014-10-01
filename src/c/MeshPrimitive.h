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
#include "Vec.h"
#include "Defines.h"
#include "Renderer.h"
#include <d3d11.h>
#include <vector>

class MeshPrimitive
{
public:
	friend class Renderer;
	~MeshPrimitive();

	Vec<float> getCenterOfMass() const {return centerOfMass;}

protected:
	MeshPrimitive(Renderer* rendererIn, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals, std::vector<Vec<float>> textureUV, Renderer::VertexShaders shader);
	MeshPrimitive::MeshPrimitive(Renderer* rendererIn, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
		std::vector<Vec<float>>& normals, Renderer::VertexShaders shader);

private:
	MeshPrimitive(){}

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	
	Renderer* renderer;
	int shaderIdx;
	size_t numFaces;
	Vec<float> centerOfMass;
};
