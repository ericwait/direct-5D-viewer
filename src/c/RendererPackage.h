#pragma once
#include "MeshPrimitive.h"
#include "Material.h"
#include "Camera.h"
#include <DirectXMath.h>

class RendererPackage
{
public:
	RendererPackage(Camera* camera){this->camera = camera;}
	~RendererPackage(){}

	void setMeshPrimitive(MeshPrimitive* mesh){meshPrimitive = mesh;}
	void setMaterial(Material* materialIn){material = materialIn;}

	void setRenderableFlag(bool on){renderable = on;}

	void setLocalToWorld(const DirectX::XMMATRIX& localToWorldIn){localToWorld = localToWorldIn;}

private:
	RendererPackage(){}

	MeshPrimitive* meshPrimitive;
	Material* material;
	DirectX::XMMATRIX localToWorld;
	Camera* camera;
	bool renderable;
};
