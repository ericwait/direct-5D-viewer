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
