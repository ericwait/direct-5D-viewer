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
