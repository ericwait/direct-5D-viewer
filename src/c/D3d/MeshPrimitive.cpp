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
#include "Global/Globals.h"
#include "Global/ErrorMsg.h"

#include <limits>

MeshPrimitive::MeshPrimitive(Renderer* rendererIn, const std::string& shaderFile, const std::string& shaderFunc)
	: renderer(rendererIn), vertShaderIdx(-1), vertexBuffer(NULL), indexBuffer(NULL)
{
	loadShader(shaderFile,shaderFunc);
}

MeshPrimitive::MeshPrimitive(Renderer * rendererIn, const std::vector<Vec<unsigned int>>& faces, const std::vector<Vec<float>>& vertices,
	const std::vector<Vec<float>>& normals, const std::vector<Vec<float>>& textureUV)
	: MeshPrimitive(rendererIn)
{
	setupMesh(faces, vertices, normals, textureUV);
}

void MeshPrimitive::setupMesh(const std::vector<Vec<unsigned int>>& facesIn, const std::vector<Vec<float>>& verticesIn,
	const std::vector<Vec<float>>& normalsIn, const std::vector<Vec<float>>& textureUVIn)
{
	faces = facesIn;
	vertices = verticesIn;
	normals = normalsIn;

	updateCenterOfMass();

	initializeResources(textureUVIn);
}

void MeshPrimitive::cleanupMesh()
{
	numFaces = 0;

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void MeshPrimitive::loadShader(const std::string& shaderFile, const std::string& shaderFunc)
{
	vertShaderIdx = renderer->registerVertexShader(shaderFile,shaderFunc);
}

void MeshPrimitive::initializeResources(const std::vector<Vec<float>>& textureUV)
{
	cleanupMesh();

	numFaces = faces.size();
	std::vector<Vertex> vertBuffer;

	vertBuffer.resize(vertices.size());
	for (int i=0; i<vertices.size(); ++i)
	{
		vertBuffer[i].pos = vertices[i];
		vertBuffer[i].normal = normals[i];
	}

	for ( int i=0; i < textureUV.size(); ++i )
		vertBuffer[i].texUV = textureUV[i];

	HRESULT hr = renderer->createVertexBuffer(vertBuffer, &vertexBuffer);
	if (FAILED(hr))
		sendHrErrMessage(hr);

	hr = renderer->createIndexBuffer(faces, &indexBuffer);
	if (FAILED(hr))
		sendHrErrMessage(hr);
}

void MeshPrimitive::updateCenterOfMass()
{
	centerOfMass = Vec<float>(0.0f, 0.0f, 0.0f);

	for ( int i=0; i < vertices.size(); ++i )
		centerOfMass += vertices[i];

	centerOfMass = centerOfMass / vertices.size();
}

bool MeshPrimitive::intersectTriangle(Vec<unsigned int> face, Vec<float> lclPntVec, Vec<float> lclDirVec, Vec<float>& triCoord)
{
	// Find vectors for two edges sharing vert0
	Vec<float> edge1(vertices[face.y]-vertices[face.x]);
	Vec<float> edge2(vertices[face.z]-vertices[face.x]);

	Vec<float> crossVec = Vec<float>::cross(lclDirVec,edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = Vec<float>::dot(edge1,crossVec);

	if( abs(det) < 1e-5f )
		return false;

	Vec<float> tvec;
	if( det > 0 )
	{
		tvec = lclPntVec - vertices[face.x];
	}
	else
	{
		tvec = lclPntVec + vertices[face.x];
		det = -det;
	}

	// Calculate U parameter and test bounds
	triCoord.x = Vec<float>::dot(tvec,crossVec);
	if( triCoord.x < 0.0f || triCoord.x > det )
		return false;

	// Prepare to test V parameter
	Vec<float> qvec = Vec<float>::cross(tvec,edge1);

	// Calculate V parameter and test bounds
	triCoord.y = Vec<float>::dot(lclDirVec,qvec);
	if( triCoord.y < 0.0f || triCoord.x + triCoord.y > det )
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	triCoord.z = Vec<float>::dot(edge2,qvec);

	triCoord = triCoord / det;

	return true;
}

bool MeshPrimitive::checkIntersect(Vec<float> lclPntVec, Vec<float> lclDirVec, float & depthOut)
{
	bool found = false;
	depthOut = std::numeric_limits<float>::max();

	for (unsigned int i=0; i < faces.size(); ++i){
		Vec<float> triCoord;

		if ( intersectTriangle(faces[i],lclPntVec,lclDirVec,triCoord) )
		{
			if ( triCoord.z < depthOut && triCoord.z > 0)
			{
				depthOut = triCoord.z;
				found = true;
			}
		}
	}

	return found;
}

MeshPrimitive::~MeshPrimitive()
{
	cleanupMesh();

	renderer = NULL;
	vertShaderIdx = -1;
}


const Vec<unsigned int> ViewAlignedPlanes::planeIndices[2] = 
{
	Vec<unsigned int>(0,2,1),
	Vec<unsigned int>(1,2,3)
};
const Vec<float> ViewAlignedPlanes::planeVertices[4] = 
{
	Vec<float>( -1.0f, -1.0f, 1.0f ), 
	Vec<float>( -1.0f, 1.0f, 1.0f), 
	Vec<float>( 1.0f, -1.0f, 1.0f), 
	Vec<float>( 1.0f, 1.0f, 1.0f)
};

ViewAlignedPlanes::ViewAlignedPlanes(Renderer * renderer, Vec<size_t> volDims, Vec<float> scaleDims)
	: dims(volDims), physicalSize(scaleDims), MeshPrimitive(renderer, "ViewAlignedVertexShader","ViewAlignedVertexShader")
{
	std::vector<Vec<float>> texUVs;
	buildViewAlignedPlanes(dims, faces, vertices, normals, texUVs);

	initializeResources(texUVs);
}

DirectX::XMMATRIX ViewAlignedPlanes::computeLocalToWorld(DirectX::XMMATRIX parentToWorld_dx)
{
	Eigen::Matrix4f parentToWorld = ConvertMatrix(parentToWorld_dx);

	Eigen::Matrix4f textureCoordinateCorrection;
	textureCoordinateCorrection<<
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f;

	Vec<float> physicalScaling = physicalSize / physicalSize.maxValue();

	Eigen::Affine3f compute(Eigen::Translation3f(0.5f, 0.5f, 0.5f));
	compute *= Eigen::Scaling(1.0f/physicalScaling.x, 1.0f/physicalScaling.y, 1.0f/physicalScaling.z);
	compute *= Eigen::Scaling(0.5f, 0.5f, 0.5f);
	compute *= textureCoordinateCorrection;
	compute *= parentToWorld.inverse();
	compute *= Eigen::Scaling(2.0f, 2.0f, 2.0f);
	compute *= Eigen::Translation3f(-0.5f, -0.5f, -0.5f);

	return ConvertMatrix(compute.matrix());
}

void ViewAlignedPlanes::buildViewAlignedPlanes(Vec<size_t> volDims, std::vector<Vec<unsigned int>>& faces,
	std::vector<Vec<float>>& vertices,std::vector<Vec<float>>& normals, std::vector<Vec<float>>& textureUV)
{
	faces.clear();
	vertices.clear();
	normals.clear();
	textureUV.clear();

	int numPlanes = int(volDims.maxValue() * 1.5f * 3.0f);//3.0 is to reduce moire
	renderer->setNumPlanes((int)(ceil(numPlanes/1.5)));

	vertices.resize(4*numPlanes);
	faces.resize(2*numPlanes);
	normals.resize(4*numPlanes);
	textureUV.resize(4*numPlanes);

	int planesFirstVert = 0;
	for (int planeIdx=0; planeIdx<numPlanes; ++planeIdx)
	{
		float zPosition = (2.0f*planeIdx) / numPlanes -1.0f;

		//i is making each plane
		for (int i=0; i<4; i++)
		{
			vertices[planesFirstVert+i] = planeVertices[i]*3.0f;
			vertices[planesFirstVert+i].z = zPosition*1.5f;

			//Vec<float> temp(vertices[planesFirstVert+i].y,vertices[planesFirstVert+i].x,vertices[planesFirstVert+i].z);
			//textureUVs[planesFirstVert+i] = temp * 0.5f + 0.5f;

			textureUV[planesFirstVert+i] = vertices[planesFirstVert+i] * 0.5f + 0.5f;
		}

		for (int i=0; i<2; ++i)
			faces[2*(numPlanes-planeIdx-1) + i] = planeIndices[i] + planesFirstVert;

		Vec<float> edge1, edge2;
		Vec<double> norm;

		edge1 = vertices[faces[2*planeIdx].y] - vertices[faces[2*planeIdx].x];
		edge2 = vertices[faces[2*planeIdx].z] - vertices[faces[2*planeIdx].x];

		Vec<float> triDir = Vec<float>::cross(edge1,edge2);

		norm = triDir.normal();

		for (int i=0; i<4 ; ++i)
			normals[planesFirstVert+i] = Vec<float>(norm);

		planesFirstVert += 4;
	}
}
