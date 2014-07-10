#pragma once
#include "MeshPrimitive.h"
#include "Material.h"
#include "Camera.h"
#include <DirectXMath.h>

class RendererPackage
{
public:
	RendererPackage(Camera* camera){this->camera = camera; label = "";}
	~RendererPackage(){}

	void setMeshPrimitive(MeshPrimitive* mesh){meshPrimitive = mesh;}
	void setMaterial(Material* materialIn){material = materialIn;}
	void setRenderableFlag(bool on){renderable = on;}
	void setLocalToWorld(const DirectX::XMMATRIX& localToWorldIn){localToWorld = localToWorldIn;}
	void setCamera(Camera* newCamera){camera = newCamera;}
	void setLabel(std::string labelIn){label = labelIn;}

	const Camera* getCamera() const{return camera;}
	Material* getMaterial() const {return material;}
	const MeshPrimitive* getMeshPrimitive() const{return meshPrimitive;}
	DirectX::XMMATRIX getLocalToWorld() const{return localToWorld;}
	bool isRenderable() const {return renderable;}
	const std::string& getLabel() const {return label;}

private:
	RendererPackage(){}

	MeshPrimitive* meshPrimitive;
	Material* material;
	DirectX::XMMATRIX localToWorld;
	Camera* camera;
	bool renderable;
	std::string label;
};
