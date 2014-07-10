#include "MeshPrimitive.h"
#include "Globals.h"

MeshPrimitive::MeshPrimitive(Renderer* rendererIn, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
	std::vector<Vec<float>>& normals,std::vector<Vec<float>> textureUV, Renderer::VertexShaders shader)
{
	renderer = rendererIn;

	std::vector<Vertex> vert;

	vert.resize(vertices.size());

	for (int i=0; i<vertices.size(); ++i)
	{
		vert[i].pos = vertices[i];
		vert[i].normal = normals[i];
		vert[i].texUV = textureUV[i];
	}

	shaderIdx = -1;

	HRESULT hr = renderer->createVertexBuffer(vert,&vertexBuffer);
	if (FAILED(hr))
		gMexMessageQueueOut.addErrorMessage(hr);

	hr = renderer->createIndexBuffer(faces,&indexBuffer);
	if (FAILED(hr))
		gMexMessageQueueOut.addErrorMessage(hr);

	numFaces = faces.size();

	shaderIdx = renderer->getVertexShader(VERTEX_SHADER_FILENAMES[shader],VERTEX_SHADER_FUNCNAMES[shader]);
}

MeshPrimitive::MeshPrimitive(Renderer* rendererIn, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
	std::vector<Vec<float>>& normals, Renderer::VertexShaders shader)
{
	renderer = rendererIn;

	std::vector<Vertex> vert;
		Vec<double> vertAcum(0,0,0);

	vert.resize(vertices.size());

	for (int i=0; i<vertices.size(); ++i)
	{		
		vertAcum += vertices[i];
		vert[i].pos = vertices[i];
		vert[i].normal = normals[i];
	}

	centerOfMass = vertAcum/vertices.size();
	shaderIdx = -1;

	HRESULT hr = renderer->createVertexBuffer(vert,&vertexBuffer);
	if (FAILED(hr))
		gMexMessageQueueOut.addErrorMessage(hr);

	hr = renderer->createIndexBuffer(faces,&indexBuffer);
	if (FAILED(hr))
		gMexMessageQueueOut.addErrorMessage(hr);

	numFaces = faces.size();

	shaderIdx = renderer->getVertexShader(VERTEX_SHADER_FILENAMES[shader],VERTEX_SHADER_FUNCNAMES[shader]);

	if (shaderIdx == -1)
		throw std::runtime_error("Cannot get vertex shader!");
}

MeshPrimitive::~MeshPrimitive()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	renderer = NULL;
	shaderIdx = -1;
}
