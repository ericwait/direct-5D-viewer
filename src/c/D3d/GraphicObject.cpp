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

#include "GraphicObject.h"
#include "..\Global\Globals.h"
#include "..\Messages\MexErrorMsg.h"
#undef min
#undef max

const Vec<unsigned int> VolumeTextureObject::triIndices[2] = 
{
	Vec<unsigned int>(0,2,1),
	Vec<unsigned int>(1,2,3)
};
const Vec<float> VolumeTextureObject::triVertices[4] = 
{
	Vec<float>( -1.0f, -1.0f, 1.0f ), 
	Vec<float>( -1.0f, 1.0f, 1.0f), 
	Vec<float>( 1.0f, -1.0f, 1.0f), 
	Vec<float>( 1.0f, 1.0f, 1.0f)
};

GraphicObject::GraphicObject()
{
	renderer = NULL;
	rendererPackage = NULL;
}

GraphicObject::GraphicObject(Renderer* renderer)
{
	this->renderer = renderer;
	rendererPackage = NULL;
}

GraphicObject::~GraphicObject()
{
	removeRendererResources();
	renderer = NULL;
}


void GraphicObject::removeRendererResources()
{
	SAFE_DELETE(rendererPackage);
}

void GraphicObject::setWireframe(bool wireframe)
{
	sendErrMessage("You cannot set wire frame on a GraphicObject!");
}

void GraphicObject::makeLocalToWorld(DirectX::XMMATRIX parentToWorld)
{
	setLocalToWorld(parentToWorld);
}

int GraphicObject::getPolygon(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut)
{
	depthOut = std::numeric_limits<float>::max();
	return -1;
}


void GraphicObject::setLocalToWorld(DirectX::XMMATRIX localToWorld)
{
	if (rendererPackage!=NULL)
		rendererPackage->setLocalToWorld(localToWorld);

	updateBoundingBox(localToWorld);
}


PolygonObject::PolygonObject()
{
	meshPrimitive = NULL;
	material = NULL;

	curBoundingBox[0] = Vec<float>(0.0f,0.0f,0.0f);
	curBoundingBox[1] = Vec<float>(0.0f,0.0f,0.0f);
	index = -1;
}

PolygonObject::PolygonObject(Renderer* rendererIn, std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices,
	std::vector<Vec<float>>& normals, Camera* camera)
{
	meshPrimitive = NULL;
	material = NULL;

	renderer = rendererIn;
	this->faces = faces;
	this->vertices = vertices;
	this->normals = normals;

	curBoundingBox[0] = vertices[0];
	curBoundingBox[1] = vertices[0];
	for (int i=1; i<vertices.size(); ++i)
	{
		curBoundingBox[0] = Vec<float>::min(curBoundingBox[0],vertices[i]);
		curBoundingBox[1] = Vec<float>::max(curBoundingBox[1],vertices[i]);
	}

	initalizeRendererResources(camera);
}

PolygonObject::~PolygonObject()
{
	//TODO remove meshPrimitive or just set to NULL

	SAFE_DELETE(material);

	faces.clear();
	vertices.clear();
	normals.clear();

	GraphicObject::~GraphicObject();
}

void PolygonObject::setColor(Vec<float> color, float alpha)
{
	if (material!=NULL)
		material->setColor(color,alpha);
}

void PolygonObject::setColorMod(Vec<float> colorMod, float alpha)
{
	if (material!=NULL)
		material->setColorModifier(colorMod,alpha);
}

void PolygonObject::setWireframe(bool wireframe)
{
	material->setWireframe(wireframe);
}

void PolygonObject::setIndex(int index)
{
	this->index = index;
}

void PolygonObject::setLabel(std::string label)
{
	rendererPackage->setLabel(label);
}

void PolygonObject::getAxisAlignedBoundingBox(Vec<float>& minVals, Vec<float>& maxVals)
{
	minVals = curBoundingBox[0];
	maxVals = curBoundingBox[1];
}

int PolygonObject::getPolygon(Vec<float> lclPntVec, Vec<float> lclDirVec, float& depthOut)
{
	depthOut = std::numeric_limits<float>::max();
	bool found = false;
	for (unsigned int i=0; i<faces.size(); ++i){
		Vec<float> triCoord;

		if (intersectTriangle(faces[i],lclPntVec,lclDirVec,triCoord))
		{
			if ( triCoord.z<depthOut && triCoord.z>0)
			{
				depthOut = triCoord.z;
				found = true;
			}
		}
	}

	if (found)
		return index;

	return -1;
}


void PolygonObject::initalizeRendererResources(Camera* camera)
{
	if (rendererPackage==NULL)
	{
		meshPrimitive = renderer->addMeshPrimitive(faces,vertices,normals,Renderer::VertexShaders::DefaultVS);
		material = new SingleColoredMaterial(renderer);//delete in destructor

		rendererPackage = new RendererPackage(camera);//delete in destructor
		rendererPackage->setMeshPrimitive(meshPrimitive);
		rendererPackage->setMaterial(material);
	}
}

void PolygonObject::updateBoundingBox(DirectX::XMMATRIX localToWorld)
{
	float mx = std::numeric_limits<float>::max();
	float mn = std::numeric_limits<float>::lowest();

	curBoundingBox[0] = Vec<float>(mx,mx,mx);
	curBoundingBox[1] = Vec<float>(mn,mn,mn);

	for (int i=0; i<vertices.size(); ++i)
	{
		DirectX::XMFLOAT3 v3(vertices[i].x,vertices[i].y,vertices[i].z);
		DirectX::XMVECTOR vV = DirectX::XMLoadFloat3(&v3);
		vV = DirectX::XMVector3TransformCoord(vV,localToWorld);

		curBoundingBox[0] = Vec<float>::min(curBoundingBox[0], Vec<float>(DirectX::XMVectorGetX(vV),DirectX::XMVectorGetY(vV),DirectX::XMVectorGetZ(vV)));
		curBoundingBox[1] = Vec<float>::max(curBoundingBox[1], Vec<float>(DirectX::XMVectorGetX(vV),DirectX::XMVectorGetY(vV),DirectX::XMVectorGetZ(vV)));
	}
}

bool PolygonObject::intersectTriangle(Vec<unsigned int> face, Vec<float> lclPntVec, Vec<float> lclDirVec, Vec<float>& triCoord)
{
// 	/* a = b - c */
// #define vector(a,b,c) \
// 	(a)[0] = (b)[0] - (c)[0];	\
// 	(a)[1] = (b)[1] - (c)[1];	\
// 	(a)[2] = (b)[2] - (c)[2];
// 
// 	int rayIntersectsTriangle(float *p, float *d,
// 		float *v0, float *v1, float *v2) {
// 
// 			float e1[3],e2[3],h[3],s[3],q[3];
// 			float a,f,u,v;
// 			vector(e1,v1,v0);
// 			vector(e2,v2,v0);
// 
// 			crossProduct(h,d,e2);
// 			a = innerProduct(e1,h);
// 
// 			if (a > -0.00001 && a < 0.00001)
// 				return(false);
// 
// 			f = 1/a;
// 			vector(s,p,v0);
// 			u = f * (innerProduct(s,h));
// 
// 			if (u < 0.0 || u > 1.0)
// 				return(false);
// 
// 			crossProduct(q,s,e1);
// 			v = f * innerProduct(d,q);
// 
// 			if (v < 0.0 || u + v > 1.0)
// 				return(false);
// 
// 			// at this stage we can compute t to find out where
// 			// the intersection point is on the line
// 			t = f * innerProduct(e2,q);
// 
// 			if (t > 0.00001) // ray intersection
// 				return(true);
// 
// 			else // this means that there is a line intersection
// 				// but not a ray intersection
// 				return (false);
// 
// 	}

	// Find vectors for two edges sharing vert0
	Vec<float> edge1(vertices[face.y]-vertices[face.x]);
	Vec<float> edge2(vertices[face.z]-vertices[face.x]);

	Vec<float> crossVec = lclDirVec.cross(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = edge1.dot(crossVec);

	if( det > -0.00001f && det < 0.00001)
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
 	triCoord.x = tvec.dot(crossVec);
 	if( triCoord.x < 0.0f || triCoord.x > det )
 		return false;
 
 	// Prepare to test V parameter
 	Vec<float> qvec = tvec.cross(edge1);
 
 	// Calculate V parameter and test bounds
 	triCoord.y = lclDirVec.dot(qvec);
 	if( triCoord.y < 0.0f || triCoord.x + triCoord.y > det )
 		return false;
 
 	// Calculate t, scale parameters, ray intersects triangle
 	triCoord.z = edge2.dot(qvec);
 
 	triCoord = triCoord / det;
 
 	return true;
}


VolumeTextureObject::VolumeTextureObject(Renderer* rendererIn, Vec<size_t> dimsIn, int numChannelsIn, unsigned char* image, 
										 Vec<float> scaleFactorIn, Camera* camera, unsigned char** constMemIn/*=NULL*/)
{
	renderer = rendererIn;
	dims = dimsIn;
	numChannels = numChannelsIn;
	scaleFactor = scaleFactorIn;

	initalizeRendererResources(camera,image, constMemIn);
}

VolumeTextureObject::~VolumeTextureObject()
{
	SAFE_DELETE(material);

	GraphicObject::~GraphicObject();
}

void VolumeTextureObject::makeLocalToWorld(DirectX::XMMATRIX parentToWorld)
{
	DirectX::XMVECTOR det;
	DirectX::XMMATRIX invParentWorld = DirectX::XMMatrixInverse(&det,parentToWorld);

	DirectX::XMMATRIX handedCorrection(0.0f,1.0f,0.0f,0.0f,
									   1.0f,0.0f,0.0f,0.0f,
									   0.0f,0.0f,1.0f,0.0f,
									   0.0f,0.0f,0.0f,1.0f);

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(-0.5f, -0.5f, -0.5f) //centering texture coord at vol origin
		* DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f) //move up to trans?
		* invParentWorld 
		* handedCorrection
		* DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f) //move up to trans?
		* DirectX::XMMatrixScaling(1.0f/scaleFactor.x, 1.0f/scaleFactor.y, 1.0f/scaleFactor.z) //convert space from world to local
		* DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.5f); //puts the origin back to 0

	DirectX::XMFLOAT3 vec(1.0f,0.0f,0.0f);
	DirectX::XMVECTOR vecU = DirectX::XMLoadFloat3(&vec);
	DirectX::XMMATRIX trans = invParentWorld * handedCorrection * DirectX::XMMatrixScaling(1.0f/dims.x,1.0f/dims.y,1.0f/dims.z);

	Vec<float> xDir, yDir, zDir;
	DirectX::XMVECTOR vecO = DirectX::XMVector3TransformNormal(vecU,trans);
	xDir = Vec<float>(DirectX::XMVectorGetX(vecO),DirectX::XMVectorGetY(vecO),DirectX::XMVectorGetZ(vecO));
	vec = DirectX::XMFLOAT3(0.0f,1.0f,0.0f);
	vecU = DirectX::XMLoadFloat3(&vec);
	vecO = DirectX::XMVector3TransformNormal(vecU,trans);
	yDir = Vec<float>(DirectX::XMVectorGetX(vecO),DirectX::XMVectorGetY(vecO),DirectX::XMVectorGetZ(vecO));
	vec = DirectX::XMFLOAT3(0.0f,0.0f,1.0f);
	vecU = DirectX::XMLoadFloat3(&vec);
	vecO = DirectX::XMVector3TransformNormal(vecU,trans);
	zDir = Vec<float>(DirectX::XMVectorGetX(vecO),DirectX::XMVectorGetY(vecO),DirectX::XMVectorGetZ(vecO));

	material->setGradientSampleDir(xDir,yDir,zDir);

	setLocalToWorld(worldMatrix);
}

void VolumeTextureObject::setWireframe(bool wireframe)
{
	sendErrMessage("You cannot set wire frame on a VolumeTextureObject!");
}

void VolumeTextureObject::initalizeRendererResources(Camera* camera, unsigned char* image, unsigned char** shaderMemIn/*=NULL*/)
{
	if (rendererPackage==NULL)
	{
		std::vector<Vec<unsigned int>> faces;
		std::vector<Vec<float>> vertices;
		std::vector<Vec<float>> normals;
		std::vector<Vec<float>> textureUVs;

		createViewAlignedPlanes(vertices, faces, normals, textureUVs);

		meshPrimitive = renderer->addMeshPrimitive(faces,vertices,normals,textureUVs,Renderer::VertexShaders::ViewAligned);
		material = new StaticVolumeTextureMaterial(renderer,dims,numChannels,image,shaderMemIn);//delete in destructor

		rendererPackage = new RendererPackage(camera);//delete in destructor
		rendererPackage->setMeshPrimitive(meshPrimitive);
		rendererPackage->setMaterial(material);
	}
}

void VolumeTextureObject::createViewAlignedPlanes(std::vector<Vec<float>> &vertices, std::vector<Vec<unsigned int>> &faces,
	std::vector<Vec<float>> &normals, std::vector<Vec<float>> &textureUVs)
{
	int numPlanes = int(dims.maxValue() * 1.5f * 1.5f);//second 1.5 is to reduce moire
	renderer->setNumPlanes((int)(ceil(numPlanes/1.5)));

	vertices.resize(4*numPlanes);
	faces.resize(2*numPlanes);
	normals.resize(4*numPlanes);
	textureUVs.resize(4*numPlanes);

	int planesFirstVert = 0;
	for (int planeIdx=0; planeIdx<numPlanes; ++planeIdx)
	{
		float zPosition = (2.0f*planeIdx) / numPlanes -1.0f;

		//i is making each plane
		for (int i=0; i<4; i++)
		{
			vertices[planesFirstVert+i] = triVertices[i]*3.0f;
			vertices[planesFirstVert+i].z = zPosition*1.5f;

			//Vec<float> temp(vertices[planesFirstVert+i].y,vertices[planesFirstVert+i].x,vertices[planesFirstVert+i].z);
			//textureUVs[planesFirstVert+i] = temp * 0.5f + 0.5f;

			textureUVs[planesFirstVert+i] = vertices[planesFirstVert+i] * 0.5f + 0.5f;
		}

		for (int i=0; i<2; ++i)
			faces[2*(numPlanes-planeIdx-1) + i] = triIndices[i] + planesFirstVert;

		Vec<float> edge1, edge2;
		Vec<double> norm;

		edge1 = vertices[faces[2*planeIdx].y] - vertices[faces[2*planeIdx].x];
		edge2 = vertices[faces[2*planeIdx].z] - vertices[faces[2*planeIdx].x];

		Vec<float> triDir = edge1.cross(edge2);

		norm = triDir.norm();

		for (int i=0; i<4 ; ++i)
			normals[planesFirstVert+i] = norm;

		planesFirstVert += 4;
	}
}

