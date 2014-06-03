#pragma once
#include "Vec.h"
#include <d3d11.h>
#include <vector>
#include "Defines.h"
#include "Renderer.h"

class MeshPrimitive
{
public:
	friend class Renderer;
	~MeshPrimitive();

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
};
