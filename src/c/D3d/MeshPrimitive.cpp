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

#include "MeshPrimitive.h"
#include "..\Global\Globals.h"
#include "..\Messages\MexErrorMsg.h"

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
		sendHrErrMessage(hr);

	hr = renderer->createIndexBuffer(faces,&indexBuffer);
	if (FAILED(hr))
		sendHrErrMessage(hr);

	numFaces = faces.size();

	std::string root = renderer->getDllDir();
	shaderIdx = renderer->getVertexShader(root+VERTEX_SHADER_FILENAMES[shader],VERTEX_SHADER_FUNCNAMES[shader]);
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
		sendHrErrMessage(hr);

	hr = renderer->createIndexBuffer(faces,&indexBuffer);
	if (FAILED(hr))
		sendHrErrMessage(hr);

	numFaces = faces.size();

	std::string root = renderer->getDllDir();
	shaderIdx = renderer->getVertexShader(root+VERTEX_SHADER_FILENAMES[shader],VERTEX_SHADER_FUNCNAMES[shader]);

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
